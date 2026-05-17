// Package health는 게이트웨이 자체 헬스체크 엔드포인트와
// 업스트림 서버 상태를 주기적으로 모니터링하는 기능을 제공한다.
package health

import (
	"context"
	"encoding/json"
	"log/slog"
	"net/http"
	"sync"
	"time"

	"github.com/makga/gateway/config"
	"github.com/makga/gateway/proxy"
)

// ─── 헬스 상태 ───────────────────────────────────────────────────────────────

// Status는 개별 업스트림 서버의 현재 상태를 나타낸다.
type Status struct {
	Addr        string        `json:"addr"`
	Healthy     bool          `json:"healthy"`
	Latency     time.Duration `json:"latency_ms"`
	FailCount   int64         `json:"fail_count"`
	LastChecked time.Time     `json:"last_checked"`
}

// GatewayHealth는 게이트웨이 전체 헬스 응답 구조체다.
type GatewayHealth struct {
	Status    string             `json:"status"`    // "healthy" | "degraded" | "unhealthy"
	Upstreams map[string]Status  `json:"upstreams"`
	Timestamp time.Time          `json:"timestamp"`
}

// ─── 헬스체크 모니터 ─────────────────────────────────────────────────────────

// Monitor는 업스트림 서버들의 상태를 주기적으로 확인한다.
// 각 업스트림에 대해 독립적인 고루틴으로 헬스체크를 수행한다.
type Monitor struct {
	dests      []*proxy.Destination
	cfg        *config.HealthCheckConfig
	clusterCfg map[string]config.ClusterConfig
	mu         sync.RWMutex
	statuses   map[string]*Status
	client     *http.Client
	stopCh     chan struct{}
}

// NewMonitor는 Monitor를 생성한다.
func NewMonitor(
	router *proxy.Router,
	cfg *config.HealthCheckConfig,
	clusterCfg map[string]config.ClusterConfig,
) *Monitor {
	return &Monitor{
		dests:      router.AllDestinations(),
		cfg:        cfg,
		clusterCfg: clusterCfg,
		statuses:   make(map[string]*Status),
		client: &http.Client{
			Timeout: time.Duration(cfg.TimeoutSec) * time.Second,
			// 리다이렉트 비활성화 (헬스체크는 직접 응답만 확인)
			CheckRedirect: func(req *http.Request, via []*http.Request) error {
				return http.ErrUseLastResponse
			},
		},
		stopCh: make(chan struct{}),
	}
}

// Start는 헬스체크 모니터를 백그라운드 고루틴으로 시작한다.
func (m *Monitor) Start(ctx context.Context) {
	if !m.cfg.Enabled {
		slog.Info("헬스체크 모니터 비활성화됨")
		return
	}

	interval := time.Duration(m.cfg.IntervalSec) * time.Second
	slog.Info("헬스체크 모니터 시작",
		"interval", interval,
		"destinations", len(m.dests),
	)

	// 시작 시 즉시 한 번 체크
	m.checkAll()

	go func() {
		ticker := time.NewTicker(interval)
		defer ticker.Stop()
		for {
			select {
			case <-ticker.C:
				m.checkAll()
			case <-ctx.Done():
				return
			case <-m.stopCh:
				return
			}
		}
	}()
}

// Stop은 헬스체크 모니터를 중지한다.
func (m *Monitor) Stop() {
	close(m.stopCh)
}

// checkAll은 모든 업스트림에 대해 병렬로 헬스체크를 수행한다.
// 각 업스트림마다 고루틴을 생성하여 동시에 체크한다.
func (m *Monitor) checkAll() {
	var wg sync.WaitGroup
	for _, dest := range m.dests {
		wg.Add(1)
		go func(d *proxy.Destination) {
			defer wg.Done()
			m.checkOne(d)
		}(dest)
	}
	wg.Wait()
}

// checkOne은 단일 업스트림에 대해 헬스체크를 수행하고 상태를 업데이트한다.
func (m *Monitor) checkOne(dest *proxy.Destination) {
	healthPath := m.cfg.Path
	// 클러스터별 헬스체크 경로 오버라이드 확인
	for _, cluster := range m.clusterCfg {
		if cluster.HealthCheckPath != "" {
			healthPath = cluster.HealthCheckPath
			break
		}
	}

	checkURL := dest.URL.String() + healthPath
	start := time.Now()

	req, err := http.NewRequestWithContext(
		context.Background(),
		http.MethodGet,
		checkURL,
		nil,
	)
	if err != nil {
		m.markUnhealthy(dest)
		return
	}
	req.Header.Set("User-Agent", "makga-gateway-healthcheck/1.0")

	resp, err := m.client.Do(req)
	latency := time.Since(start)

	addrKey := dest.URL.String()

	if err != nil || resp.StatusCode >= 500 {
		if resp != nil {
			resp.Body.Close()
		}
		m.markUnhealthy(dest)
		m.updateStatus(addrKey, false, latency, dest.FailCount.Load())
		return
	}
	resp.Body.Close()

	m.markHealthy(dest)
	m.updateStatus(addrKey, true, latency, dest.FailCount.Load())
}

// markUnhealthy는 연속 실패 횟수를 증가시키고 임계값 초과 시 비정상 마킹한다.
func (m *Monitor) markUnhealthy(dest *proxy.Destination) {
	fails := dest.FailCount.Add(1)
	dest.SuccessCount.Store(0)
	if fails >= int64(m.cfg.UnhealthyThreshold) {
		if dest.Healthy.CompareAndSwap(true, false) {
			slog.Warn("업스트림 비정상 마킹",
				"addr", dest.URL.String(),
				"consecutive_failures", fails,
			)
		}
	}
}

// markHealthy는 연속 성공 횟수를 증가시키고 임계값 초과 시 정상 마킹한다.
func (m *Monitor) markHealthy(dest *proxy.Destination) {
	successes := dest.SuccessCount.Add(1)
	dest.FailCount.Store(0)
	if successes >= int64(m.cfg.HealthyThreshold) {
		if dest.Healthy.CompareAndSwap(false, true) {
			slog.Info("업스트림 정상 복구",
				"addr", dest.URL.String(),
				"consecutive_successes", successes,
			)
		}
	}
}

func (m *Monitor) updateStatus(addr string, healthy bool, latency time.Duration, failCount int64) {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.statuses[addr] = &Status{
		Addr:        addr,
		Healthy:     healthy,
		Latency:     latency / time.Millisecond,
		FailCount:   failCount,
		LastChecked: time.Now(),
	}
}

// ─── HTTP 핸들러 ─────────────────────────────────────────────────────────────

// Handler는 /health 엔드포인트 핸들러를 반환한다.
func (m *Monitor) Handler() http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		m.mu.RLock()
		statuses := make(map[string]Status, len(m.statuses))
		for k, v := range m.statuses {
			statuses[k] = *v
		}
		m.mu.RUnlock()

		// 전체 상태 판단
		totalHealthy := 0
		for _, s := range statuses {
			if s.Healthy {
				totalHealthy++
			}
		}

		overallStatus := "healthy"
		httpStatus := http.StatusOK
		switch {
		case len(statuses) > 0 && totalHealthy == 0:
			overallStatus = "unhealthy"
			httpStatus = http.StatusServiceUnavailable
		case totalHealthy < len(statuses):
			overallStatus = "degraded"
			httpStatus = http.StatusOK // degraded 상태에서도 200 반환 (로드밸런서 헬스체크 통과)
		}

		resp := GatewayHealth{
			Status:    overallStatus,
			Upstreams: statuses,
			Timestamp: time.Now(),
		}

		w.Header().Set("Content-Type", "application/json")
		w.WriteHeader(httpStatus)
		json.NewEncoder(w).Encode(resp) //nolint:errcheck
	}
}

// ReadinessHandler는 /ready 엔드포인트 핸들러를 반환한다.
// Kubernetes readinessProbe에서 사용한다.
func (m *Monitor) ReadinessHandler() http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		// 최소 1개 이상의 정상 업스트림이 있으면 Ready
		for _, dest := range m.dests {
			if dest.Healthy.Load() {
				w.WriteHeader(http.StatusOK)
				w.Write([]byte(`{"status":"ready"}`)) //nolint:errcheck
				return
			}
		}
		w.WriteHeader(http.StatusServiceUnavailable)
		w.Write([]byte(`{"status":"not ready","reason":"no healthy upstreams"}`)) //nolint:errcheck
	}
}

// LivenessHandler는 /live 엔드포인트 핸들러를 반환한다.
// Kubernetes livenessProbe에서 사용한다. 게이트웨이 프로세스 자체가 살아있으면 200을 반환한다.
func LivenessHandler() http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		w.WriteHeader(http.StatusOK)
		w.Write([]byte(`{"status":"alive"}`)) //nolint:errcheck
	}
}
