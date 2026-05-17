package proxy

import (
	"bufio"
	"context"
	"fmt"
	"io"
	"log/slog"
	"net"
	"net/http"
	"net/http/httputil"
	"strings"
	"time"

	"github.com/makga/gateway/config"
	"github.com/sony/gobreaker"
)

// ─── 프록시 핸들러 ───────────────────────────────────────────────────────────

// Handler는 게이트웨이의 핵심 HTTP 핸들러다.
// 라우팅, 로드밸런싱, 서킷브레이커, WebSocket 프록시를 담당한다.
type Handler struct {
	router   *Router
	cbs      map[string]*gobreaker.CircuitBreaker // 클러스터별 서킷브레이커
	cfg      *config.Config
	transport http.RoundTripper
}

// NewHandler는 Handler를 생성한다.
func NewHandler(router *Router, cfg *config.Config) *Handler {
	// 고루틴 대량 처리를 위한 커스텀 Transport 설정
	// MaxIdleConnsPerHost를 높여 연결 재사용률을 극대화한다.
	transport := &http.Transport{
		MaxIdleConns:          cfg.Server.MaxConcurrency,
		MaxIdleConnsPerHost:   512,
		MaxConnsPerHost:       0, // 무제한
		IdleConnTimeout:       90 * time.Second,
		TLSHandshakeTimeout:   10 * time.Second,
		ExpectContinueTimeout: 1 * time.Second,
		DisableCompression:    false,
		ForceAttemptHTTP2:     true,
		DialContext: (&net.Dialer{
			Timeout:   30 * time.Second,
			KeepAlive: 30 * time.Second,
		}).DialContext,
	}

	// 클러스터별 서킷브레이커 생성
	cbCfg := cfg.CircuitBreaker
	cbs := make(map[string]*gobreaker.CircuitBreaker, len(cfg.Clusters))
	for name := range cfg.Clusters {
		clusterName := name // 클로저 캡처
		settings := gobreaker.Settings{
			Name:        "cb-" + clusterName,
			MaxRequests: cbCfg.HalfOpenMaxRequests,
			Interval:    0, // Closed 상태 카운터 리셋 비활성화
			Timeout:     time.Duration(cbCfg.OpenTimeoutSec) * time.Second,
			ReadyToTrip: func(counts gobreaker.Counts) bool {
				return counts.ConsecutiveFailures >= uint32(cbCfg.MaxConsecutiveFailures)
			},
			OnStateChange: func(name string, from, to gobreaker.State) {
				slog.Warn("서킷브레이커 상태 변경",
					"cluster", clusterName,
					"from", from.String(),
					"to", to.String(),
				)
			},
		}
		cbs[name] = gobreaker.NewCircuitBreaker(settings)
	}

	return &Handler{
		router:    router,
		cbs:       cbs,
		cfg:       cfg,
		transport: transport,
	}
}

// ServeHTTP는 http.Handler 인터페이스를 구현한다.
// 각 요청은 독립적인 고루틴에서 처리된다 (Go net/http 기본 동작).
func (h *Handler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	route := h.router.Match(r.URL.Path)
	if route == nil {
		writeJSON(w, http.StatusNotFound, `{"error":"route not found","path":"`+r.URL.Path+`"}`)
		return
	}

	// 업스트림 선택
	dest := route.LB.Next()
	if dest == nil {
		writeJSON(w, http.StatusServiceUnavailable, `{"error":"no healthy upstream","cluster":"`+route.Cluster+`"}`)
		return
	}

	// WebSocket 요청 처리
	if route.WebSocket && IsWebSocket(r) {
		h.proxyWebSocket(w, r, dest)
		return
	}

	// 서킷브레이커를 통한 HTTP 프록시
	cb, ok := h.cbs[route.Cluster]
	if !ok {
		h.proxyHTTP(w, r, dest, route)
		return
	}

	_, err := cb.Execute(func() (interface{}, error) {
		return nil, h.proxyHTTP(w, r, dest, route)
	})
	if err != nil {
		if err == gobreaker.ErrOpenState {
			writeJSON(w, http.StatusServiceUnavailable,
				fmt.Sprintf(`{"error":"circuit open","cluster":"%s"}`, route.Cluster))
			return
		}
		// proxyHTTP 내부에서 이미 응답을 작성한 경우 추가 처리 불필요
	}
}

// ─── HTTP 프록시 ─────────────────────────────────────────────────────────────

func (h *Handler) proxyHTTP(w http.ResponseWriter, r *http.Request, dest *Destination, route *Route) error {
	// 활성 연결 수 추적 (Least-Connection 알고리즘 지원)
	dest.ActiveConns.Add(1)
	defer dest.ActiveConns.Add(-1)

	// 업스트림 URL 조합
	target := *dest.URL
	target.Path = singleJoiningSlash(dest.URL.Path, r.URL.Path)
	if route.StripPrefix {
		target.Path = strings.TrimPrefix(target.Path, route.PathPrefix)
		if target.Path == "" {
			target.Path = "/"
		}
	}
	target.RawQuery = r.URL.RawQuery

	// httputil.ReverseProxy 사용 (표준 라이브러리, 고루틴 안전)
	proxy := &httputil.ReverseProxy{
		Director: func(req *http.Request) {
			req.URL = &target
			req.Host = dest.URL.Host

			// 클라이언트 IP 전달
			if clientIP, _, err := net.SplitHostPort(r.RemoteAddr); err == nil {
				if prior := req.Header.Get("X-Forwarded-For"); prior != "" {
					clientIP = prior + ", " + clientIP
				}
				req.Header.Set("X-Forwarded-For", clientIP)
			}
			req.Header.Set("X-Forwarded-Host", r.Host)
			req.Header.Set("X-Forwarded-Proto", scheme(r))

			// 인증 정보 전달 (미들웨어에서 설정한 컨텍스트 값)
			if uid, ok := r.Context().Value(CtxKeyAccountUID).(string); ok && uid != "" {
				req.Header.Set("X-Account-Uid", uid)
			}
			if role, ok := r.Context().Value(CtxKeyRole).(string); ok && role != "" {
				req.Header.Set("X-Account-Role", role)
			}
			if reqID, ok := r.Context().Value(CtxKeyRequestID).(string); ok && reqID != "" {
				req.Header.Set("X-Request-ID", reqID)
			}
		},
		Transport: h.transport,
		ErrorHandler: func(w http.ResponseWriter, r *http.Request, err error) {
			slog.Error("업스트림 오류", "dest", dest.URL.String(), "err", err)
			writeJSON(w, http.StatusBadGateway, `{"error":"upstream error"}`)
		},
	}

	proxy.ServeHTTP(w, r)
	return nil
}

// ─── WebSocket 프록시 ────────────────────────────────────────────────────────

// proxyWebSocket은 WebSocket 연결을 업스트림으로 터널링한다.
// 각 연결에 대해 2개의 고루틴(클라이언트→업스트림, 업스트림→클라이언트)이 생성된다.
func (h *Handler) proxyWebSocket(w http.ResponseWriter, r *http.Request, dest *Destination) {
	target := *dest.URL
	target.Scheme = "ws"
	if dest.URL.Scheme == "https" {
		target.Scheme = "wss"
	}
	target.Path = singleJoiningSlash(dest.URL.Path, r.URL.Path)
	target.RawQuery = r.URL.RawQuery

	// 업스트림 TCP 연결
	upstreamConn, err := net.DialTimeout("tcp", dest.URL.Host, 10*time.Second)
	if err != nil {
		writeJSON(w, http.StatusBadGateway, `{"error":"websocket upstream unreachable"}`)
		return
	}
	defer upstreamConn.Close()

	// 클라이언트 연결 하이재킹
	hijacker, ok := w.(http.Hijacker)
	if !ok {
		writeJSON(w, http.StatusInternalServerError, `{"error":"websocket not supported"}`)
		return
	}
	clientConn, _, err := hijacker.Hijack()
	if err != nil {
		return
	}
	defer clientConn.Close()

	// 업스트림으로 HTTP Upgrade 요청 전달
	upgradeReq := r.Clone(context.Background())
	upgradeReq.URL = &target
	upgradeReq.Host = dest.URL.Host
	if err := upgradeReq.Write(upstreamConn); err != nil {
		return
	}

	// 업스트림 응답 확인
	resp, err := http.ReadResponse(bufio.NewReader(upstreamConn), upgradeReq)
	if err != nil || resp.StatusCode != http.StatusSwitchingProtocols {
		return
	}
	if err := resp.Write(clientConn); err != nil {
		return
	}

	dest.ActiveConns.Add(1)
	defer dest.ActiveConns.Add(-1)

	// 양방향 데이터 중계 (각각 고루틴으로 병렬 처리)
	done := make(chan struct{}, 2)
	go func() {
		io.Copy(upstreamConn, clientConn) //nolint:errcheck
		done <- struct{}{}
	}()
	go func() {
		io.Copy(clientConn, upstreamConn) //nolint:errcheck
		done <- struct{}{}
	}()
	<-done // 어느 한 쪽이 끊어지면 종료
}

// ─── 유틸리티 ────────────────────────────────────────────────────────────────

func writeJSON(w http.ResponseWriter, status int, body string) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(status)
	w.Write([]byte(body)) //nolint:errcheck
}

func singleJoiningSlash(a, b string) string {
	aSlash := strings.HasSuffix(a, "/")
	bSlash := strings.HasPrefix(b, "/")
	switch {
	case aSlash && bSlash:
		return a + b[1:]
	case !aSlash && !bSlash:
		return a + "/" + b
	}
	return a + b
}

func scheme(r *http.Request) string {
	if r.TLS != nil {
		return "https"
	}
	if proto := r.Header.Get("X-Forwarded-Proto"); proto != "" {
		return proto
	}
	return "http"
}
