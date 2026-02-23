-- Admin tool setup: run once to create the admin_accounts table and default superadmin
-- Default credentials: admin / admin1234  (change after first login!)

CREATE TABLE IF NOT EXISTS `admin_accounts` (
  `id`            INT           NOT NULL AUTO_INCREMENT,
  `username`      VARCHAR(64)   NOT NULL,
  `password_hash` VARCHAR(256)  NOT NULL,
  `role`          ENUM('superadmin','admin','viewer') NOT NULL DEFAULT 'viewer',
  `is_active`     TINYINT(1)    NOT NULL DEFAULT 1,
  `last_login`    DATETIME      DEFAULT NULL,
  `created_at`    DATETIME      NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_username` (`username`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- BCrypt hash of "admin1234" (cost=12) — change immediately after first login
INSERT IGNORE INTO `admin_accounts` (`username`, `password_hash`, `role`)
VALUES (
  'admin',
  '$2a$12$92IXUNpkjO0rOQ5byMi.Ye4oKoEa3Ro9llC/.og/at2.uheWG/igi',
  'superadmin'
);
