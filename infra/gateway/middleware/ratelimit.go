package middleware

import (
	"context"
	"fmt"
	"log/slog"
	"net"
	"net/http"
	"strconv"
	"strings"
	"time"

	"github.com/makga/gateway/config"
	"github.com/makga/gateway/proxy"
	"github.com/redis/go-redis/v9"
)

// ─── 슬라이딩 윈도우 레이트 리밋 ────────────────────────────────────────────
//
// Redis ZSET을 사용한 슬라이딩 윈도우 알고리즘:
//   1. 현재 윈도우 범위 밖의 오래된 항목을 제거 (ZREMRANGEBYSCORE)
//   2. 현재 윈도우 내 요청 수를 카운트 (ZCARD)
//   3. 한도 초과 시 429 반환
//   4. 한도 미만 시 현재 타임스탬프를 추가 (ZADD)
//   5. TTL 갱신 (EXPIRE)
//
// 이 방식은 고루틴에서 동시에 호출되어도 Redis 원자성으로 안전하다.

var slidingWindowScript = redis.NewScript(`
local key = KEYS[1]
local now = tonumber(ARGV[1])
local window = tonumber(ARGV[2])
local limit = tonumber(ARGV[3])
local expire = tonumber(ARGV[4])

-- 윈도우 밖 항목 제거
redis.call('ZREMRANGEBYSCORE', key, '-inf', now - window * 1000)

-- 현재 카운트 확인
local count = redis.call('ZCARD', key)
if count >= limit then
  return {0, count, limit}
end

-- 현재 요청 추가 (score: 밀리초 타임스탬프, member: 고유 식별자)
redis.call('ZADD', key, now, now .. '-' .. math.random(1000000))
redis.call('EXPIRE', key, expire)
return {1, count + 1, limit}
`)

// ─── RateLimiter ─────────────────────────────────────────────────────────────

// RateLimiter는 Redis 기반 슬라이딩 윈도우 레이트 리밋 미들웨어다.
type RateLimiter struct {
	rdb    redis.UniversalClient
	cfg    *config.RateLimitConfig
	// 경로별 오버라이드 맵 (빠른 조회)
	pathOverrides map[string]*config.PathRateLimit
}

// NewRateLimiter는 RateLimiter를 생성한다.
func NewRateLimiter(rdb redis.UniversalClient, cfg *config.RateLimitConfig) *RateLimiter {
	overrides := make(map[string]*config.PathRateLimit, len(cfg.PathOverrides))
	for i := range cfg.PathOverrides {
		overrides[cfg.PathOverrides[i].Path] = &cfg.PathOverrides[i]
	}
	return &RateLimiter{rdb: rdb, cfg: cfg, pathOverrides: overrides}
}

// Handler는 레이트 리밋 미들웨어 핸들러를 반환한다.
func (rl *RateLimiter) Handler(next http.Handler) http.Handler {
	if !rl.cfg.Enabled {
		return next
	}

	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		ctx := r.Context()

		// 경로별 오버라이드 확인
		var requests int
		var windowSec int

		if override, ok := rl.pathOverrides[r.URL.Path]; ok {
			requests = override.Requests
			windowSec = override.WindowSec
		} else {
			// 인증된 사용자는 per_user 한도, 아니면 per_ip 한도 적용
			if uid, ok := ctx.Value(proxy.CtxKeyAccountUID).(string); ok && uid != "" {
				requests = rl.cfg.PerUser.Requests
				windowSec = rl.cfg.PerUser.WindowSec
			} else {
				requests = rl.cfg.PerIP.Requests
				windowSec = rl.cfg.PerIP.WindowSec
			}
		}

		key := rl.buildKey(r)
		allowed, count, limit, err := rl.check(ctx, key, requests, windowSec)
		if err != nil {
			// Redis 오류 시 요청 통과 (fail-open 정책)
			slog.Warn("레이트리밋 Redis 오류, 요청 통과", "err", err)
			next.ServeHTTP(w, r)
			return
		}

		// 응답 헤더에 레이트 리밋 정보 추가
		w.Header().Set("X-RateLimit-Limit", strconv.Itoa(limit))
		w.Header().Set("X-RateLimit-Remaining", strconv.Itoa(max(0, limit-count)))
		w.Header().Set("X-RateLimit-Window", strconv.Itoa(windowSec))

		if !allowed {
			w.Header().Set("Retry-After", strconv.Itoa(windowSec))
			writeJSON(w, http.StatusTooManyRequests,
				fmt.Sprintf(`{"error":"rate limit exceeded","limit":%d,"window_sec":%d}`, limit, windowSec))
			return
		}

		next.ServeHTTP(w, r)
	})
}

// check는 Redis 슬라이딩 윈도우 스크립트를 실행하여 요청 허용 여부를 반환한다.
// 반환값: (allowed, currentCount, limit, error)
func (rl *RateLimiter) check(ctx context.Context, key string, limit, windowSec int) (bool, int, int, error) {
	now := time.Now().UnixMilli()
	expire := windowSec + 1

	result, err := slidingWindowScript.Run(ctx, rl.rdb, []string{key},
		now, windowSec, limit, expire,
	).Int64Slice()
	if err != nil {
		return true, 0, limit, err
	}
	if len(result) < 3 {
		return true, 0, limit, nil
	}

	allowed := result[0] == 1
	count := int(result[1])
	return allowed, count, int(result[2]), nil
}

// buildKey는 레이트 리밋 Redis 키를 생성한다.
// 인증된 사용자는 UID 기반, 비인증 요청은 IP 기반 키를 사용한다.
func (rl *RateLimiter) buildKey(r *http.Request) string {
	prefix := rl.cfg.RedisKeyPrefix

	// 인증된 사용자 키
	if uid, ok := r.Context().Value(proxy.CtxKeyAccountUID).(string); ok && uid != "" {
		return prefix + "user:" + uid
	}

	// IP 기반 키
	ip := extractClientIP(r)
	// 경로별 오버라이드가 있으면 경로도 키에 포함
	if _, ok := rl.pathOverrides[r.URL.Path]; ok {
		path := strings.ReplaceAll(r.URL.Path, "/", "_")
		return prefix + "ip:" + ip + ":path:" + path
	}
	return prefix + "ip:" + ip
}

// extractClientIP는 요청에서 클라이언트 IP를 추출한다.
// X-Forwarded-For → X-Real-IP → RemoteAddr 순으로 확인한다.
func extractClientIP(r *http.Request) string {
	if xff := r.Header.Get("X-Forwarded-For"); xff != "" {
		// 첫 번째 IP가 실제 클라이언트 IP
		parts := strings.SplitN(xff, ",", 2)
		return strings.TrimSpace(parts[0])
	}
	if xri := r.Header.Get("X-Real-IP"); xri != "" {
		return xri
	}
	ip, _, err := net.SplitHostPort(r.RemoteAddr)
	if err != nil {
		return r.RemoteAddr
	}
	return ip
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}
