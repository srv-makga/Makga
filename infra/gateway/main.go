// Makga API Gateway
//
// Go 기반 고성능 API 게이트웨이.
// 고루틴을 활용한 대량 동시 요청 처리, 고가용성(HA) 지원.
//
// 주요 기능:
//   - 경로 기반 리버스 프록시 (HTTP + WebSocket)
//   - JWT 인증 및 역할 기반 접근 제어
//   - Redis 기반 슬라이딩 윈도우 레이트 리밋
//   - 서킷브레이커 (gobreaker)
//   - Round-Robin / Least-Connection 로드밸런서
//   - 업스트림 헬스체크 (고루틴 병렬 수행)
//   - Graceful Shutdown (SIGINT/SIGTERM)
//   - 구조화 로깅 (slog JSON)
package main

import (
	"context"
	"flag"
	"fmt"
	"log/slog"
	"net/http"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/makga/gateway/config"
	"github.com/makga/gateway/health"
	"github.com/makga/gateway/middleware"
	"github.com/makga/gateway/proxy"
	"github.com/redis/go-redis/v9"
)

// ─── 엔트리포인트 ────────────────────────────────────────────────────────────

func main() {
	// 플래그 파싱
	configPath := flag.String("config", "config/gateway.yaml", "설정 파일 경로")
	flag.Parse()

	// 설정 로드
	cfg, err := config.Load(*configPath)
	if err != nil {
		slog.Error("설정 로드 실패", "err", err)
		os.Exit(1)
	}

	// 로거 초기화
	initLogger(cfg.Logging)

	slog.Info("Makga API Gateway 시작",
		"port", cfg.Server.Port,
		"max_concurrency", cfg.Server.MaxConcurrency,
		"routes", len(cfg.Routes),
		"clusters", len(cfg.Clusters),
	)

	// Redis 클라이언트 초기화
	rdb, err := newRedisClient(&cfg.Redis)
	if err != nil {
		slog.Error("Redis 연결 실패", "err", err)
		os.Exit(1)
	}
	defer rdb.Close()

	// 라우터 빌드
	router, err := proxy.NewRouter(cfg)
	if err != nil {
		slog.Error("라우터 빌드 실패", "err", err)
		os.Exit(1)
	}

	// 헬스체크 모니터 시작
	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	monitor := health.NewMonitor(router, &cfg.HealthCheck, cfg.Clusters)
	monitor.Start(ctx)

	// 미들웨어 체인 구성
	proxyHandler := proxy.NewHandler(router, cfg)
	authMW := middleware.NewAuthMiddleware(&cfg.JWT)
	rateLimiter := middleware.NewRateLimiter(rdb, &cfg.RateLimit)

	// 메인 핸들러: 미들웨어 체인 적용
	// 순서: Logger → CORS → RateLimit → Auth(경로별) → Proxy
	mainHandler := buildMainHandler(proxyHandler, authMW, rateLimiter, router, cfg)

	// HTTP 서버 설정
	// Go net/http는 각 요청을 독립적인 고루틴에서 처리하므로
	// MaxConcurrency 설정은 업스트림 연결 풀 크기를 제어한다.
	srv := &http.Server{
		Addr:         fmt.Sprintf(":%d", cfg.Server.Port),
		Handler:      mainHandler,
		ReadTimeout:  time.Duration(cfg.Server.ReadTimeoutSec) * time.Second,
		WriteTimeout: time.Duration(cfg.Server.WriteTimeoutSec) * time.Second,
		IdleTimeout:  time.Duration(cfg.Server.IdleTimeoutSec) * time.Second,
		// 최대 헤더 크기 제한 (기본 1MB)
		MaxHeaderBytes: 1 << 20,
	}

	// 헬스체크/준비성/생존성 엔드포인트 등록
	mux := http.NewServeMux()
	mux.Handle("/health", monitor.Handler())
	mux.Handle("/ready", monitor.ReadinessHandler())
	mux.Handle("/live", health.LivenessHandler())
	mux.Handle("/", mainHandler)
	srv.Handler = mux

	// 서버 시작 (별도 고루틴)
	serverErr := make(chan error, 1)
	go func() {
		slog.Info("HTTP 서버 리스닝", "addr", srv.Addr)
		if err := srv.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			serverErr <- err
		}
	}()

	// ─── Graceful Shutdown ───────────────────────────────────────────────────
	// SIGINT(Ctrl+C) 또는 SIGTERM(Kubernetes pod 종료) 신호를 받으면
	// 진행 중인 요청이 완료될 때까지 대기한 후 종료한다.
	quit := make(chan os.Signal, 1)
	signal.Notify(quit, syscall.SIGINT, syscall.SIGTERM)

	select {
	case sig := <-quit:
		slog.Info("종료 신호 수신", "signal", sig.String())
	case err := <-serverErr:
		slog.Error("서버 오류", "err", err)
	}

	slog.Info("Graceful Shutdown 시작",
		"timeout_sec", cfg.Server.ShutdownTimeoutSec,
	)

	shutdownCtx, shutdownCancel := context.WithTimeout(
		context.Background(),
		time.Duration(cfg.Server.ShutdownTimeoutSec)*time.Second,
	)
	defer shutdownCancel()

	// 헬스체크 모니터 중지
	monitor.Stop()
	cancel()

	// HTTP 서버 종료 (진행 중인 요청 완료 대기)
	if err := srv.Shutdown(shutdownCtx); err != nil {
		slog.Error("Graceful Shutdown 실패", "err", err)
		os.Exit(1)
	}

	slog.Info("게이트웨이 정상 종료")
}

// ─── 미들웨어 체인 빌더 ──────────────────────────────────────────────────────

// buildMainHandler는 미들웨어 체인을 구성한다.
// 경로별로 인증 필요 여부가 다르므로, 라우터 정보를 기반으로 동적으로 적용한다.
func buildMainHandler(
	proxyHandler *proxy.Handler,
	authMW *middleware.AuthMiddleware,
	rateLimiter *middleware.RateLimiter,
	router *proxy.Router,
	cfg *config.Config,
) http.Handler {
	// 경로별 인증 래퍼
	routeAwareHandler := http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		route := router.Match(r.URL.Path)

		var authRequired bool
		var requiredRole string
		if route != nil {
			authRequired = route.AuthRequired
			requiredRole = route.RequiredRole
		}

		// 인증 미들웨어 적용 후 프록시 핸들러 호출
		authMW.Wrap(proxyHandler, authRequired, requiredRole).ServeHTTP(w, r)
	})

	// 미들웨어 체인: Logger → CORS → RateLimit → Auth+Proxy
	var handler http.Handler = routeAwareHandler
	handler = rateLimiter.Handler(handler)
	if cfg.CORS.Enabled {
		handler = middleware.CORS(
			cfg.CORS.AllowedOrigins,
			cfg.CORS.AllowedMethods,
			cfg.CORS.AllowedHeaders,
			cfg.CORS.MaxAgeSec,
		)(handler)
	}
	handler = middleware.Logger(cfg.Logging.SlowRequestThresholdMs)(handler)

	return handler
}

// ─── Redis 클라이언트 팩토리 ─────────────────────────────────────────────────

func newRedisClient(cfg *config.RedisConfig) (redis.UniversalClient, error) {
	var rdb redis.UniversalClient

	if cfg.ClusterMode {
		rdb = redis.NewClusterClient(&redis.ClusterOptions{
			Addrs:        cfg.ClusterAddrs,
			Password:     cfg.Password,
			PoolSize:     cfg.PoolSize,
			MinIdleConns: cfg.MinIdleConns,
			DialTimeout:  time.Duration(cfg.DialTimeoutSec) * time.Second,
			ReadTimeout:  time.Duration(cfg.ReadTimeoutSec) * time.Second,
			WriteTimeout: time.Duration(cfg.WriteTimeoutSec) * time.Second,
		})
	} else {
		rdb = redis.NewClient(&redis.Options{
			Addr:         cfg.Addr,
			Password:     cfg.Password,
			DB:           cfg.DB,
			PoolSize:     cfg.PoolSize,
			MinIdleConns: cfg.MinIdleConns,
			DialTimeout:  time.Duration(cfg.DialTimeoutSec) * time.Second,
			ReadTimeout:  time.Duration(cfg.ReadTimeoutSec) * time.Second,
			WriteTimeout: time.Duration(cfg.WriteTimeoutSec) * time.Second,
		})
	}

	// 연결 확인
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()
	if err := rdb.Ping(ctx).Err(); err != nil {
		return nil, fmt.Errorf("Redis ping 실패: %w", err)
	}

	slog.Info("Redis 연결 성공",
		"cluster_mode", cfg.ClusterMode,
		"addr", cfg.Addr,
	)
	return rdb, nil
}

// ─── 로거 초기화 ─────────────────────────────────────────────────────────────

func initLogger(cfg config.LoggingConfig) {
	var level slog.Level
	switch cfg.Level {
	case "debug":
		level = slog.LevelDebug
	case "warn":
		level = slog.LevelWarn
	case "error":
		level = slog.LevelError
	default:
		level = slog.LevelInfo
	}

	opts := &slog.HandlerOptions{Level: level}
	var handler slog.Handler
	if cfg.Format == "json" {
		handler = slog.NewJSONHandler(os.Stdout, opts)
	} else {
		handler = slog.NewTextHandler(os.Stdout, opts)
	}
	slog.SetDefault(slog.New(handler))
}
