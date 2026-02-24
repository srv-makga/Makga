package balancer

import "sync/atomic"

// LeastConn picks the backend with the fewest active connections.
type LeastConn struct{}

func (l *LeastConn) Next(backends []*Backend) *Backend {
	if len(backends) == 0 {
		return nil
	}
	var chosen *Backend
	var min int64 = -1
	for _, b := range backends {
		c := atomic.LoadInt64(&b.ActiveConns)
		if min < 0 || c < min {
			min = c
			chosen = b
		}
	}
	return chosen
}
