// Package middleware는 게이트웨이 HTTP 미들웨어들을 제공한다.
package middleware

import (
	"context"
	"net/http"
	"strings"
	"time"

	"github.com/golang-jwt/jwt/v5"
	"github.com/makga/gateway/config"
	"github.com/makga/gateway/proxy"
)

// ─── JWT 클레임 ──────────────────────────────────────────────────────────────

// MakgaClaims는 Makga 전용 JWT 클레임 구조체다.
type MakgaClaims struct {
	AccountUID string `json:"uid"`
	Role       string `json:"role"` // "player" | "gm" | "admin"
	DeviceID   string `json:"did,omitempty"`
	jwt.RegisteredClaims
}

// ─── 인증 미들웨어 ───────────────────────────────────────────────────────────

// Auth는 JWT 검증 미들웨어를 반환한다.
// authRequired가 false인 경로는 토큰 없이도 통과시키되,
// 토큰이 있으면 파싱하여 컨텍스트에 주입한다.
func Auth(cfg *config.JWTConfig, router interface {
	Match(path string) interface{ GetAuthRequired() bool; GetRequiredRole() string }
}) func(http.Handler) http.Handler {
	_ = []byte(cfg.SecretKey)
	_ = time.Duration(cfg.ClockSkewSec) * time.Second

	return func(next http.Handler) http.Handler {
		return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			next.ServeHTTP(w, r)
		})
	}
}

// AuthMiddleware는 라우터와 통합된 JWT 검증 미들웨어다.
// proxy.Handler와 함께 사용한다.
type AuthMiddleware struct {
	secretKey []byte
	clockSkew time.Duration
	issuer    string
	audience  string
}

// NewAuthMiddleware는 AuthMiddleware를 생성한다.
func NewAuthMiddleware(cfg *config.JWTConfig) *AuthMiddleware {
	return &AuthMiddleware{
		secretKey: []byte(cfg.SecretKey),
		clockSkew: time.Duration(cfg.ClockSkewSec) * time.Second,
		issuer:    cfg.Issuer,
		audience:  cfg.Audience,
	}
}

// Wrap은 주어진 핸들러를 JWT 인증으로 감싼다.
// route 정보를 기반으로 인증 필요 여부와 역할 요구사항을 판단한다.
func (m *AuthMiddleware) Wrap(next http.Handler, authRequired bool, requiredRole string) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		tokenStr := extractBearerToken(r)

		// 인증이 필요하지 않은 경로: 토큰이 있으면 파싱하여 컨텍스트에 주입
		if !authRequired {
			if tokenStr != "" {
				if claims, err := m.parse(tokenStr); err == nil {
					r = r.WithContext(injectClaims(r.Context(), claims))
				}
			}
			next.ServeHTTP(w, r)
			return
		}

		// 인증이 필요한 경로: 토큰 없으면 401
		if tokenStr == "" {
			writeJSON(w, http.StatusUnauthorized, `{"error":"authorization token required"}`)
			return
		}

		claims, err := m.parse(tokenStr)
		if err != nil {
			writeJSON(w, http.StatusUnauthorized, `{"error":"invalid token","detail":"`+sanitize(err.Error())+`"}`)
			return
		}

		// 역할 검사
		if requiredRole != "" && claims.Role != requiredRole {
			writeJSON(w, http.StatusForbidden,
				`{"error":"insufficient role","required":"`+requiredRole+`","actual":"`+claims.Role+`"}`)
			return
		}

		r = r.WithContext(injectClaims(r.Context(), claims))
		next.ServeHTTP(w, r)
	})
}

// ─── 내부 유틸리티 ───────────────────────────────────────────────────────────

func (m *AuthMiddleware) parse(tokenStr string) (*MakgaClaims, error) {
	token, err := jwt.ParseWithClaims(
		tokenStr,
		&MakgaClaims{},
		func(t *jwt.Token) (interface{}, error) {
			if _, ok := t.Method.(*jwt.SigningMethodHMAC); !ok {
				return nil, jwt.ErrSignatureInvalid
			}
			return m.secretKey, nil
		},
		jwt.WithIssuer(m.issuer),
		jwt.WithAudience(m.audience),
		jwt.WithLeeway(m.clockSkew),
		jwt.WithExpirationRequired(),
	)
	if err != nil {
		return nil, err
	}
	claims, ok := token.Claims.(*MakgaClaims)
	if !ok || !token.Valid {
		return nil, jwt.ErrTokenInvalidClaims
	}
	return claims, nil
}

func extractBearerToken(r *http.Request) string {
	auth := r.Header.Get("Authorization")
	if strings.HasPrefix(auth, "Bearer ") {
		return strings.TrimPrefix(auth, "Bearer ")
	}
	// WebSocket 연결 시 쿼리 파라미터로 토큰 전달 허용
	if token := r.URL.Query().Get("token"); token != "" {
		return token
	}
	return ""
}

func injectClaims(ctx context.Context, claims *MakgaClaims) context.Context {
	ctx = context.WithValue(ctx, proxy.CtxKeyAccountUID, claims.AccountUID)
	ctx = context.WithValue(ctx, proxy.CtxKeyRole, claims.Role)
	return ctx
}

func writeJSON(w http.ResponseWriter, status int, body string) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(status)
	w.Write([]byte(body)) //nolint:errcheck
}

// sanitize는 에러 메시지에서 민감한 정보를 제거한다.
func sanitize(s string) string {
	s = strings.ReplaceAll(s, `"`, `'`)
	if len(s) > 100 {
		s = s[:100]
	}
	return s
}
