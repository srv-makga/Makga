package proxy

import (
	"context"
	"fmt"
	"log"
	"net"
	"net/http"
	"net/http/httputil"
	"net/url"

	"lb/balancer"
	"lb/health"
)

// HTTPProxy is an L7 reverse proxy for a single frontend listener.
type HTTPProxy struct {
	listen  string
	pool    *health.Pool
	lb      balancer.Balancer
}

// NewHTTPProxy creates an HTTPProxy.
func NewHTTPProxy(listen string, pool *health.Pool, lb balancer.Balancer) *HTTPProxy {
	return &HTTPProxy{listen: listen, pool: pool, lb: lb}
}

// Run starts listening and blocks until ctx is cancelled.
func (h *HTTPProxy) Run(ctx context.Context) error {
	ln, err := net.Listen("tcp", h.listen)
	if err != nil {
		return fmt.Errorf("http_proxy: listen %s: %w", h.listen, err)
	}
	log.Printf("[http] listening on %s", h.listen)

	srv := &http.Server{
		Handler: http.HandlerFunc(h.handle),
	}

	go func() {
		<-ctx.Done()
		srv.Shutdown(context.Background()) //nolint:errcheck
	}()

	if err := srv.Serve(ln); err != nil && err != http.ErrServerClosed {
		return fmt.Errorf("http_proxy: serve: %w", err)
	}
	return nil
}

func (h *HTTPProxy) handle(w http.ResponseWriter, r *http.Request) {
	backends := h.pool.Healthy()
	b := h.lb.Next(backends)
	if nil == b {
		http.Error(w, "no healthy backends", http.StatusServiceUnavailable)
		return
	}

	target, err := url.Parse("http://" + b.Address)
	if err != nil {
		http.Error(w, "bad backend address", http.StatusInternalServerError)
		return
	}

	rp := httputil.NewSingleHostReverseProxy(target)
	rp.ErrorHandler = func(w http.ResponseWriter, r *http.Request, err error) {
		log.Printf("[http] proxy error → %s: %v", b.Address, err)
		http.Error(w, "backend error", http.StatusBadGateway)
	}
	rp.ServeHTTP(w, r)
}
