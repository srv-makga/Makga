package proxy

import (
	"net/http"
	"net/url"
	"sort"
	"strings"

	"github.com/makga/gateway/config"
)

// ─── 라우팅 테이블 ───────────────────────────────────────────────────────────

// Route는 컴파일된 라우팅 규칙을 나타낸다.
type Route struct {
	PathPrefix   string
	Cluster      string
	AuthRequired bool
	RequiredRole string
	WebSocket    bool
	StripPrefix  bool
	LB           LoadBalancer
}

// Router는 요청 경로를 분석하여 적절한 업스트림을 선택한다.
// 접두사가 긴 규칙이 우선 매칭된다 (가장 구체적인 경로 우선).
type Router struct {
	routes []*Route // 접두사 길이 내림차순 정렬
}

// NewRouter는 설정에서 라우팅 테이블을 빌드한다.
func NewRouter(cfg *config.Config) (*Router, error) {
	routes := make([]*Route, 0, len(cfg.Routes))

	for _, rc := range cfg.Routes {
		cluster, ok := cfg.Clusters[rc.Cluster]
		if !ok {
			continue
		}

		// 업스트림 Destination 슬라이스 생성
		dests := make([]*Destination, 0, len(cluster.Destinations))
		for _, dc := range cluster.Destinations {
			u, err := url.Parse(dc.Addr)
			if err != nil {
				return nil, err
			}
			w := dc.Weight
			if w <= 0 {
				w = 1
			}
			d := &Destination{URL: u, Weight: w}
			d.Healthy.Store(true) // 초기 상태는 정상으로 가정
			dests = append(dests, d)
		}

		lb := NewLoadBalancer(cluster.LoadBalance, dests)

		routes = append(routes, &Route{
			PathPrefix:   rc.PathPrefix,
			Cluster:      rc.Cluster,
			AuthRequired: rc.AuthRequired,
			RequiredRole: rc.RequiredRole,
			WebSocket:    rc.WebSocket,
			StripPrefix:  rc.StripPrefix,
			LB:           lb,
		})
	}

	// 접두사 길이 내림차순 정렬 → 더 구체적인 경로가 먼저 매칭됨
	sort.Slice(routes, func(i, j int) bool {
		return len(routes[i].PathPrefix) > len(routes[j].PathPrefix)
	})

	return &Router{routes: routes}, nil
}

// Match는 요청 경로에 매칭되는 Route를 반환한다.
// 매칭되는 규칙이 없으면 nil을 반환한다.
func (r *Router) Match(path string) *Route {
	for _, route := range r.routes {
		if strings.HasPrefix(path, route.PathPrefix) {
			return route
		}
	}
	return nil
}

// AllDestinations는 모든 클러스터의 모든 Destination 목록을 반환한다.
// 헬스체크 루프에서 사용한다.
func (r *Router) AllDestinations() []*Destination {
	seen := make(map[*Destination]bool)
	var all []*Destination
	for _, route := range r.routes {
		for _, d := range route.LB.Destinations() {
			if !seen[d] {
				seen[d] = true
				all = append(all, d)
			}
		}
	}
	return all
}

// IsWebSocket은 요청이 WebSocket 업그레이드 요청인지 확인한다.
func IsWebSocket(r *http.Request) bool {
	return strings.EqualFold(r.Header.Get("Upgrade"), "websocket")
}
