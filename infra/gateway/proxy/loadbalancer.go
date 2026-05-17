// Package proxy는 리버스 프록시 핵심 로직을 담는다.
package proxy

import (
	"math/rand"
	"net/url"
	"sync"
	"sync/atomic"
)

// ─── 업스트림 대상 ───────────────────────────────────────────────────────────

// Destination은 단일 업스트림 서버를 나타낸다.
type Destination struct {
	URL          *url.URL
	Weight       int
	Healthy      atomic.Bool
	ActiveConns  atomic.Int64  // Least-Connection 알고리즘에서 사용
	FailCount    atomic.Int64  // 헬스체크 연속 실패 횟수
	SuccessCount atomic.Int64  // 헬스체크 연속 성공 횟수
}

// ─── 로드밸런서 인터페이스 ───────────────────────────────────────────────────

// LoadBalancer는 업스트림 선택 알고리즘 인터페이스다.
type LoadBalancer interface {
	// Next는 다음 요청을 처리할 업스트림을 반환한다.
	// 정상 업스트림이 없으면 nil을 반환한다.
	Next() *Destination
	// Destinations는 전체 업스트림 목록을 반환한다.
	Destinations() []*Destination
}

// ─── Round-Robin ─────────────────────────────────────────────────────────────

// RoundRobinLB는 정상 업스트림을 순서대로 선택한다.
type RoundRobinLB struct {
	dests  []*Destination
	cursor atomic.Uint64
}

func NewRoundRobinLB(dests []*Destination) *RoundRobinLB {
	return &RoundRobinLB{dests: dests}
}

func (lb *RoundRobinLB) Next() *Destination {
	healthy := lb.healthyDests()
	if len(healthy) == 0 {
		return nil
	}
	idx := lb.cursor.Add(1) - 1
	return healthy[idx%uint64(len(healthy))]
}

func (lb *RoundRobinLB) Destinations() []*Destination { return lb.dests }

func (lb *RoundRobinLB) healthyDests() []*Destination {
	out := make([]*Destination, 0, len(lb.dests))
	for _, d := range lb.dests {
		if d.Healthy.Load() {
			out = append(out, d)
		}
	}
	return out
}

// ─── Least-Connection ────────────────────────────────────────────────────────

// LeastConnLB는 현재 활성 연결 수가 가장 적은 정상 업스트림을 선택한다.
type LeastConnLB struct {
	dests []*Destination
	mu    sync.Mutex
}

func NewLeastConnLB(dests []*Destination) *LeastConnLB {
	return &LeastConnLB{dests: dests}
}

func (lb *LeastConnLB) Next() *Destination {
	lb.mu.Lock()
	defer lb.mu.Unlock()

	var best *Destination
	var bestConns int64 = -1

	for _, d := range lb.dests {
		if !d.Healthy.Load() {
			continue
		}
		conns := d.ActiveConns.Load()
		if bestConns < 0 || conns < bestConns {
			bestConns = conns
			best = d
		}
	}
	return best
}

func (lb *LeastConnLB) Destinations() []*Destination { return lb.dests }

// ─── Random ──────────────────────────────────────────────────────────────────

// RandomLB는 정상 업스트림 중 하나를 무작위로 선택한다.
type RandomLB struct {
	dests []*Destination
}

func NewRandomLB(dests []*Destination) *RandomLB {
	return &RandomLB{dests: dests}
}

func (lb *RandomLB) Next() *Destination {
	healthy := make([]*Destination, 0, len(lb.dests))
	for _, d := range lb.dests {
		if d.Healthy.Load() {
			healthy = append(healthy, d)
		}
	}
	if len(healthy) == 0 {
		return nil
	}
	return healthy[rand.Intn(len(healthy))]
}

func (lb *RandomLB) Destinations() []*Destination { return lb.dests }

// ─── 팩토리 ──────────────────────────────────────────────────────────────────

// NewLoadBalancer는 알고리즘 이름에 따라 적절한 LoadBalancer를 생성한다.
func NewLoadBalancer(algorithm string, dests []*Destination) LoadBalancer {
	switch algorithm {
	case "least_conn":
		return NewLeastConnLB(dests)
	case "random":
		return NewRandomLB(dests)
	default: // "round_robin" 또는 빈 문자열
		return NewRoundRobinLB(dests)
	}
}
