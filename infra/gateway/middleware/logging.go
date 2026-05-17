package middleware

import (
	"context"
	"crypto/rand"
	"encoding/hex"
	"log/slog"
	"net/http"
	"time"

	"github.com/makga/gateway/proxy"
)

// ─── 응답 래퍼 ───────────────────────────────────────────────────────────────

// responseWriter는 상태 코드와 응답 크기를 캡처하기 위한 래퍼다.
type responseWriter struct {
	http.ResponseWriter
	statusCode int
	written    int64
}

func newResponseWriter(w http.ResponseWriter) *responseWriter {
	return &responseWriter{ResponseWriter: w, statusCode: http.StatusOK}
}

func (rw *responseWriter) WriteHeader(code int) {
	rw.statusCode = code
	rw.ResponseWriter.WriteHeader(code)
}

func (rw *responseWriter) Write(b []byte) (int, error) {
	n, err := rw.ResponseWriter.Write(b)
	rw.written += int64(n)
	return n, err
}

// ─── 로깅 미들웨어 ───────────────────────────────────────────────────────────

// Logger는 요청/응답 로깅 미들웨어를 반환한다.
// 각 요청에 고유한 X-Request-ID를 부여하고, 처리 시간을 기록한다.
func Logger(slowThresholdMs int) func(http.Handler) http.Handler {
	return func(next http.Handler) http.Handler {
		return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			start := time.Now()

			// Request-ID 생성 또는 전달
			reqID := r.Header.Get("X-Request-ID")
			if reqID == "" {
				reqID = generateRequestID()
			}
			w.Header().Set("X-Request-ID", reqID)

			// 컨텍스트에 Request-ID 주입
			ctx := context.WithValue(r.Context(), proxy.CtxKeyRequestID, reqID)
			r = r.WithContext(ctx)

			rw := newResponseWriter(w)
			next.ServeHTTP(rw, r)

			duration := time.Since(start)
			durationMs := duration.Milliseconds()

			attrs := []any{
				"request_id", reqID,
				"method", r.Method,
				"path", r.URL.Path,
				"status", rw.statusCode,
				"duration_ms", durationMs,
				"bytes", rw.written,
				"remote_addr", extractClientIP(r),
				"user_agent", r.UserAgent(),
			}

			// 인증된 사용자 UID 추가
			if uid, ok := ctx.Value(proxy.CtxKeyAccountUID).(string); ok && uid != "" {
				attrs = append(attrs, "uid", uid)
			}

			// 느린 요청 경고
			if slowThresholdMs > 0 && durationMs >= int64(slowThresholdMs) {
				slog.Warn("느린 요청 감지", attrs...)
				return
			}

			// 상태 코드에 따른 로그 레벨 분류
			switch {
			case rw.statusCode >= 500:
				slog.Error("요청 처리", attrs...)
			case rw.statusCode >= 400:
				slog.Warn("요청 처리", attrs...)
			default:
				slog.Info("요청 처리", attrs...)
			}
		})
	}
}

// ─── CORS 미들웨어 ───────────────────────────────────────────────────────────

// CORS는 Cross-Origin Resource Sharing 미들웨어를 반환한다.
func CORS(allowedOrigins, allowedMethods, allowedHeaders []string, maxAgeSec int) func(http.Handler) http.Handler {
	originsMap := make(map[string]bool, len(allowedOrigins))
	for _, o := range allowedOrigins {
		originsMap[o] = true
	}
	methodsStr := joinStrings(allowedMethods)
	headersStr := joinStrings(allowedHeaders)
	maxAgeStr := itoa(maxAgeSec)

	return func(next http.Handler) http.Handler {
		return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			origin := r.Header.Get("Origin")
			if origin != "" && originsMap[origin] {
				w.Header().Set("Access-Control-Allow-Origin", origin)
				w.Header().Set("Access-Control-Allow-Methods", methodsStr)
				w.Header().Set("Access-Control-Allow-Headers", headersStr)
				w.Header().Set("Access-Control-Max-Age", maxAgeStr)
				w.Header().Set("Vary", "Origin")
			}

			if r.Method == http.MethodOptions {
				w.WriteHeader(http.StatusNoContent)
				return
			}

			next.ServeHTTP(w, r)
		})
	}
}

// ─── 유틸리티 ────────────────────────────────────────────────────────────────

func generateRequestID() string {
	b := make([]byte, 8)
	rand.Read(b) //nolint:errcheck
	return hex.EncodeToString(b)
}

func joinStrings(ss []string) string {
	result := ""
	for i, s := range ss {
		if i > 0 {
			result += ", "
		}
		result += s
	}
	return result
}

func itoa(n int) string {
	if n == 0 {
		return "0"
	}
	result := ""
	for n > 0 {
		result = string(rune('0'+n%10)) + result
		n /= 10
	}
	return result
}
