package balancer

import "sync/atomic"

// RoundRobin distributes requests evenly across backends in order.
type RoundRobin struct {
	counter uint64
}

func (r *RoundRobin) Next(backends []*Backend) *Backend {
	if len(backends) == 0 {
		return nil
	}
	idx := atomic.AddUint64(&r.counter, 1) - 1
	return backends[idx%uint64(len(backends))]
}
