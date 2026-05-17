// Package config는 gateway.yaml 설정 파일을 로드하고 구조체로 파싱한다.
// 환경변수 ${VAR:default} 형태의 플레이스홀더를 자동으로 치환한다.
package config

import (
	"fmt"
	"os"
	"regexp"
	"strings"

	"gopkg.in/yaml.v3"
)

// ─── 최상위 설정 구조체 ──────────────────────────────────────────────────────

// Config는 gateway.yaml 전체 설정을 담는 루트 구조체다.
type Config struct {
	Server         ServerConfig                 `yaml:"server"`
	JWT            JWTConfig                    `yaml:"jwt"`
	RateLimit      RateLimitConfig              `yaml:"rate_limit"`
	CircuitBreaker CircuitBreakerConfig         `yaml:"circuit_breaker"`
	HealthCheck    HealthCheckConfig            `yaml:"health_check"`
	Redis          RedisConfig                  `yaml:"redis"`
	Routes         []RouteConfig                `yaml:"routes"`
	Clusters       map[string]ClusterConfig     `yaml:"clusters"`
	Logging        LoggingConfig                `yaml:"logging"`
	CORS           CORSConfig                   `yaml:"cors"`
}

// ─── 서버 설정 ───────────────────────────────────────────────────────────────

type ServerConfig struct {
	Port                int `yaml:"port"`
	ReadTimeoutSec      int `yaml:"read_timeout_sec"`
	WriteTimeoutSec     int `yaml:"write_timeout_sec"`
	IdleTimeoutSec      int `yaml:"idle_timeout_sec"`
	MaxConcurrency      int `yaml:"max_concurrency"`
	ShutdownTimeoutSec  int `yaml:"shutdown_timeout_sec"`
	MaxBodyBytes        int `yaml:"max_body_bytes"`
}

// ─── JWT 설정 ────────────────────────────────────────────────────────────────

type JWTConfig struct {
	Issuer       string `yaml:"issuer"`
	Audience     string `yaml:"audience"`
	SecretKey    string `yaml:"secret_key"`
	ClockSkewSec int    `yaml:"clock_skew_sec"`
}

// ─── 레이트 리밋 설정 ────────────────────────────────────────────────────────

type RateLimitConfig struct {
	Enabled        bool              `yaml:"enabled"`
	RedisKeyPrefix string            `yaml:"redis_key_prefix"`
	PerIP          RateLimitRule     `yaml:"per_ip"`
	PerUser        RateLimitRule     `yaml:"per_user"`
	PathOverrides  []PathRateLimit   `yaml:"path_overrides"`
}

type RateLimitRule struct {
	Requests  int `yaml:"requests"`
	WindowSec int `yaml:"window_sec"`
}

type PathRateLimit struct {
	Path      string `yaml:"path"`
	Requests  int    `yaml:"requests"`
	WindowSec int    `yaml:"window_sec"`
}

// ─── 서킷브레이커 설정 ───────────────────────────────────────────────────────

type CircuitBreakerConfig struct {
	MaxConsecutiveFailures int     `yaml:"max_consecutive_failures"`
	OpenTimeoutSec         int     `yaml:"open_timeout_sec"`
	HalfOpenMaxRequests    uint32  `yaml:"half_open_max_requests"`
	SuccessRateThreshold   float64 `yaml:"success_rate_threshold"`
}

// ─── 헬스체크 설정 ───────────────────────────────────────────────────────────

type HealthCheckConfig struct {
	Enabled            bool   `yaml:"enabled"`
	IntervalSec        int    `yaml:"interval_sec"`
	TimeoutSec         int    `yaml:"timeout_sec"`
	UnhealthyThreshold int    `yaml:"unhealthy_threshold"`
	HealthyThreshold   int    `yaml:"healthy_threshold"`
	Path               string `yaml:"path"`
}

// ─── Redis 설정 ──────────────────────────────────────────────────────────────

type RedisConfig struct {
	ClusterMode    bool     `yaml:"cluster_mode"`
	Addr           string   `yaml:"addr"`
	ClusterAddrs   []string `yaml:"cluster_addrs"`
	Password       string   `yaml:"password"`
	DB             int      `yaml:"db"`
	PoolSize       int      `yaml:"pool_size"`
	MinIdleConns   int      `yaml:"min_idle_conns"`
	DialTimeoutSec int      `yaml:"dial_timeout_sec"`
	ReadTimeoutSec int      `yaml:"read_timeout_sec"`
	WriteTimeoutSec int     `yaml:"write_timeout_sec"`
}

// ─── 라우팅 설정 ─────────────────────────────────────────────────────────────

// RouteConfig는 단일 라우팅 규칙을 정의한다.
type RouteConfig struct {
	PathPrefix   string `yaml:"path_prefix"`
	Cluster      string `yaml:"cluster"`
	AuthRequired bool   `yaml:"auth_required"`
	RequiredRole string `yaml:"required_role"`
	WebSocket    bool   `yaml:"websocket"`
	StripPrefix  bool   `yaml:"strip_prefix"`
}

// ─── 클러스터 설정 ───────────────────────────────────────────────────────────

// ClusterConfig는 업스트림 서버 클러스터를 정의한다.
type ClusterConfig struct {
	LoadBalance      string              `yaml:"load_balance"`
	Destinations     []DestinationConfig `yaml:"destinations"`
	HealthCheckPath  string              `yaml:"health_check_path"`
}

// DestinationConfig는 개별 업스트림 서버 주소를 정의한다.
type DestinationConfig struct {
	Addr   string `yaml:"addr"`
	Weight int    `yaml:"weight"` // 0이면 기본값 1로 처리
}

// ─── 로깅 설정 ───────────────────────────────────────────────────────────────

type LoggingConfig struct {
	Level                  string `yaml:"level"`
	Format                 string `yaml:"format"`
	SlowRequestThresholdMs int    `yaml:"slow_request_threshold_ms"`
}

// ─── CORS 설정 ───────────────────────────────────────────────────────────────

type CORSConfig struct {
	Enabled        bool     `yaml:"enabled"`
	AllowedOrigins []string `yaml:"allowed_origins"`
	AllowedMethods []string `yaml:"allowed_methods"`
	AllowedHeaders []string `yaml:"allowed_headers"`
	MaxAgeSec      int      `yaml:"max_age_sec"`
}

// ─── 로더 ────────────────────────────────────────────────────────────────────

var envVarPattern = regexp.MustCompile(`\$\{([^}:]+)(?::([^}]*))?\}`)

// expandEnv는 ${VAR} 또는 ${VAR:default} 형태의 플레이스홀더를 환경변수로 치환한다.
func expandEnv(s string) string {
	return envVarPattern.ReplaceAllStringFunc(s, func(match string) string {
		sub := envVarPattern.FindStringSubmatch(match)
		if len(sub) < 2 {
			return match
		}
		varName := sub[1]
		defaultVal := ""
		if len(sub) >= 3 {
			defaultVal = sub[2]
		}
		if val, ok := os.LookupEnv(varName); ok {
			return val
		}
		return defaultVal
	})
}

// Load는 지정된 경로의 YAML 파일을 읽어 Config 구조체로 파싱한다.
// 환경변수 플레이스홀더를 자동으로 치환한다.
func Load(path string) (*Config, error) {
	raw, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("config: 파일 읽기 실패 (%s): %w", path, err)
	}

	// 환경변수 치환
	expanded := expandEnv(string(raw))

	var cfg Config
	if err := yaml.Unmarshal([]byte(expanded), &cfg); err != nil {
		return nil, fmt.Errorf("config: YAML 파싱 실패: %w", err)
	}

	if err := validate(&cfg); err != nil {
		return nil, fmt.Errorf("config: 유효성 검사 실패: %w", err)
	}

	return &cfg, nil
}

// validate는 필수 설정값이 존재하는지 검사한다.
func validate(cfg *Config) error {
	if cfg.Server.Port == 0 {
		return fmt.Errorf("server.port 가 설정되지 않았습니다")
	}
	if cfg.JWT.SecretKey == "" {
		return fmt.Errorf("jwt.secret_key 가 설정되지 않았습니다")
	}
	if len(cfg.Routes) == 0 {
		return fmt.Errorf("라우팅 규칙이 하나도 정의되지 않았습니다")
	}
	for i, r := range cfg.Routes {
		if r.PathPrefix == "" {
			return fmt.Errorf("routes[%d].path_prefix 가 비어있습니다", i)
		}
		if r.Cluster == "" {
			return fmt.Errorf("routes[%d].cluster 가 비어있습니다", i)
		}
		if _, ok := cfg.Clusters[r.Cluster]; !ok {
			return fmt.Errorf("routes[%d] 의 cluster '%s' 가 clusters 섹션에 정의되지 않았습니다", i, r.Cluster)
		}
	}
	for name, cluster := range cfg.Clusters {
		if len(cluster.Destinations) == 0 {
			return fmt.Errorf("cluster '%s' 에 destination 이 없습니다", name)
		}
		lb := strings.ToLower(cluster.LoadBalance)
		if lb != "" && lb != "round_robin" && lb != "least_conn" && lb != "random" {
			return fmt.Errorf("cluster '%s' 의 load_balance 값 '%s' 이 유효하지 않습니다", name, cluster.LoadBalance)
		}
	}
	return nil
}
