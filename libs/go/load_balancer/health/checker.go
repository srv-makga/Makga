package health

import (
	"context"
	"log"
	"sync"
	"sync/atomic"
	"time"

	"lb/balancer"
	"lb/config"
)

// Pool manages a set of backends with live health state.
type Pool struct {
	mu       sync.RWMutex
	all      []*balancer.Backend // all backends (healthy + unhealthy)
	alive    []*balancer.Backend // only healthy backends (rebuilt on each check)
	flags    []atomic.Bool       // flags[i] == true → all[i] is healthy
}

// NewPool builds a Pool from config backends and starts background health checks.
func NewPool(ctx context.Context, cfg config.FrontendConfig) *Pool {
	p := &Pool{
		all:   make([]*balancer.Backend, len(cfg.Backends)),
		flags: make([]atomic.Bool, len(cfg.Backends)),
	}
	for i, b := range cfg.Backends {
		p.all[i] = &balancer.Backend{Address: b.Address}
		p.flags[i].Store(true) // assume healthy at startup
	}
	p.rebuild()

	go p.run(ctx, cfg.HealthCheckInterval())
	return p
}

// Healthy returns the current slice of healthy backends (read-only, do not modify).
func (p *Pool) Healthy() []*balancer.Backend {
	p.mu.RLock()
	defer p.mu.RUnlock()
	return p.alive
}

// run periodically probes every backend and rebuilds the healthy list.
func (p *Pool) run(ctx context.Context, interval time.Duration) {
	ticker := time.NewTicker(interval)
	defer ticker.Stop()
	for {
		select {
		case <-ctx.Done():
			return
		case <-ticker.C:
			p.checkAll()
		}
	}
}

func (p *Pool) checkAll() {
	var wg sync.WaitGroup
	for i, b := range p.all {
		wg.Add(1)
		go func(idx int, b *balancer.Backend) {
			defer wg.Done()
			ok := balancer.Alive(b.Address)
			prev := p.flags[idx].Swap(ok)
			if prev != ok {
				if ok {
					log.Printf("[health] backend %s is UP", b.Address)
				} else {
					log.Printf("[health] backend %s is DOWN", b.Address)
				}
			}
		}(i, b)
	}
	wg.Wait()
	p.rebuild()
}

func (p *Pool) rebuild() {
	alive := make([]*balancer.Backend, 0, len(p.all))
	for i, b := range p.all {
		if p.flags[i].Load() {
			alive = append(alive, b)
		}
	}
	p.mu.Lock()
	p.alive = alive
	p.mu.Unlock()
}
