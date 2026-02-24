package main

import (
	"context"
	"flag"
	"log"
	"os"
	"os/signal"
	"sync"
	"syscall"

	"lb/balancer"
	"lb/config"
	"lb/health"
	"lb/proxy"
)

func main() {
	cfgPath := flag.String("config", "config.json", "path to JSON config file")
	flag.Parse()

	cfg, err := config.Load(*cfgPath)
	if err != nil {
		log.Fatalf("failed to load config: %v", err)
	}

	ctx, cancel := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
	defer cancel()

	var wg sync.WaitGroup
	for _, f := range cfg.Frontends {
		f := f // capture loop var

		pool := health.NewPool(ctx, f)
		lb := balancer.New(f.Balancer)

		wg.Add(1)
		go func() {
			defer wg.Done()
			var runErr error
			switch f.Protocol {
			case "http":
				runErr = proxy.NewHTTPProxy(f.Listen, pool, lb).Run(ctx)
			case "tcp":
				runErr = proxy.NewTCPProxy(f.Listen, pool, lb).Run(ctx)
			default:
				log.Printf("unknown protocol %q for %s, skipping", f.Protocol, f.Listen)
				return
			}
			if runErr != nil {
				log.Printf("frontend %s exited: %v", f.Listen, runErr)
			}
		}()
	}

	<-ctx.Done()
	log.Println("shutting down...")
	wg.Wait()
	log.Println("bye")
}
