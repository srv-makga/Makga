package balancer

import "net"

// Backend wraps an upstream address with connection tracking.
type Backend struct {
	Address     string
	ActiveConns int64 // atomic; managed by LeastConn
}

// Balancer selects a healthy backend for each new connection/request.
type Balancer interface {
	// Next returns the next backend to use, or nil if none are available.
	Next(backends []*Backend) *Backend
}

// New creates a Balancer by name ("round_robin" | "least_conn").
// Falls back to round-robin for unknown names.
func New(name string) Balancer {
	switch name {
	case "least_conn":
		return &LeastConn{}
	default:
		return &RoundRobin{}
	}
}

// Alive dials the backend address to check if it is reachable.
func Alive(address string) bool {
	conn, err := net.Dial("tcp", address)
	if err != nil {
		return false
	}
	conn.Close()
	return true
}
