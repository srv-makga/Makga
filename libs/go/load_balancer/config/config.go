package config

import (
	"encoding/json"
	"fmt"
	"os"
	"time"
)

// BackendConfig represents a single upstream server.
type BackendConfig struct {
	Address string `json:"address"` // "ip:port"
}

// FrontendConfig is one listener → many backends mapping.
type FrontendConfig struct {
	Protocol        string          `json:"protocol"`         // "http" | "tcp"
	Listen          string          `json:"listen"`           // "ip:port"
	Balancer        string          `json:"balancer"`         // "round_robin" | "least_conn"
	Backends        []BackendConfig `json:"backends"`
	HealthCheckSecs int             `json:"health_check_secs"` // 0 → use default (5)
}

// Config is the top-level JSON config.
type Config struct {
	Frontends []FrontendConfig `json:"frontends"`
}

func (c *FrontendConfig) HealthCheckInterval() time.Duration {
	if c.HealthCheckSecs <= 0 {
		return 5 * time.Second
	}
	return time.Duration(c.HealthCheckSecs) * time.Second
}

// Load reads and parses the JSON config file at path.
func Load(path string) (*Config, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("config: read %q: %w", path, err)
	}
	var cfg Config
	if err := json.Unmarshal(data, &cfg); err != nil {
		return nil, fmt.Errorf("config: parse: %w", err)
	}
	if err := cfg.validate(); err != nil {
		return nil, err
	}
	return &cfg, nil
}

func (c *Config) validate() error {
	for i, f := range c.Frontends {
		if f.Listen == "" {
			return fmt.Errorf("config: frontend[%d]: listen is empty", i)
		}
		if f.Protocol != "http" && f.Protocol != "tcp" {
			return fmt.Errorf("config: frontend[%d]: protocol must be http or tcp, got %q", i, f.Protocol)
		}
		if len(f.Backends) == 0 {
			return fmt.Errorf("config: frontend[%d]: no backends defined", i)
		}
	}
	return nil
}
