package proxy

import (
	"context"
	"fmt"
	"io"
	"log"
	"net"
	"sync/atomic"
	"time"

	"lb/balancer"
	"lb/health"
)

// TCPProxy is an L4 TCP proxy for a single frontend listener.
type TCPProxy struct {
	listen string
	pool   *health.Pool
	lb     balancer.Balancer
}

// NewTCPProxy creates a TCPProxy.
func NewTCPProxy(listen string, pool *health.Pool, lb balancer.Balancer) *TCPProxy {
	return &TCPProxy{listen: listen, pool: pool, lb: lb}
}

// Run starts listening and blocks until ctx is cancelled.
func (t *TCPProxy) Run(ctx context.Context) error {
	ln, err := net.Listen("tcp", t.listen)
	if err != nil {
		return fmt.Errorf("tcp_proxy: listen %s: %w", t.listen, err)
	}
	log.Printf("[tcp] listening on %s", t.listen)

	go func() {
		<-ctx.Done()
		ln.Close()
	}()

	for {
		conn, err := ln.Accept()
		if err != nil {
			select {
			case <-ctx.Done():
				return nil // normal shutdown
			default:
				log.Printf("[tcp] accept error: %v", err)
				time.Sleep(10 * time.Millisecond)
				continue
			}
		}
		go t.handle(conn)
	}
}

func (t *TCPProxy) handle(client net.Conn) {
	defer client.Close()

	backends := t.pool.Healthy()
	b := t.lb.Next(backends)
	if nil == b {
		log.Printf("[tcp] no healthy backends for %s", client.RemoteAddr())
		return
	}

	upstream, err := net.Dial("tcp", b.Address)
	if err != nil {
		log.Printf("[tcp] dial %s: %v", b.Address, err)
		return
	}
	defer upstream.Close()

	atomic.AddInt64(&b.ActiveConns, 1)
	defer atomic.AddInt64(&b.ActiveConns, -1)

	done := make(chan struct{}, 2)
	copy := func(dst, src net.Conn) {
		io.Copy(dst, src) //nolint:errcheck
		done <- struct{}{}
	}

	go copy(upstream, client)
	go copy(client, upstream)

	// wait for either direction to close
	<-done
}
