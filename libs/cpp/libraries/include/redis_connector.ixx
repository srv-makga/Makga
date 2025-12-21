#include <memory>
#include <optional>
#include <stdexcept>

#include "../../3rdparty/hiredis/hiredis.h"

export module makga.lib.database.redis.connector;

import <string>;
import <chrono>;
import makga.lib.lock;

export namespace makga::lib::database {
class RedisReplyDeleter
{
public:
	RedisReplyDeleter() = default;
	~RedisReplyDeleter() = default;

	void operator()(redisReply* reply) const noexcept
	{
		if (nullptr != reply)
		{
			::freeReplyObject(reply);
		}
	}
};

export class RedisConnector
{
	using UniqueRedisReply = std::unique_ptr<redisReply, RedisReplyDeleter>;
	using SharedRedisReply = std::shared_ptr<redisReply>;

public:
	// @param host: Redis 서버 주소 (aka 127.0.0.1)
	// @param port: Redis 포트 (aka 6379)
	RedisConnector(std::string_view host, int port);

	virtual ~RedisConnector() noexcept;

	bool Connect();
	void Disconnect() noexcept;
	bool IsConnected() const;

	// Common
	std::optional<std::string> Get(std::string_view key);
	std::optional<std::string> GetDel(std::string_view key); // 데이터 조회 후 삭제, Redis 6.2+
	bool Set(std::string_view key, std::string_view value, std::chrono::milliseconds ttl = std::chrono::milliseconds::zero());
	bool Del(std::string_view key);

	bool SetEx(std::string_view key, std::string_view value, std::chrono::milliseconds ttl); // 만료 시간과 함께 설정

	bool Exists(std::string_view key);
	void Unlink(std::string_view key);

	template<typename... Args>
	bool MSet(std::string_view key, std::string_view value, ...);
	std::vector<std::string> MGet(const std::vector<std::string>& keys);

	uint64_t Incr(std::string_view key);
	uint64_t IncrBy(std::string_view key);
	uint64_t Decr(std::string_view key);
	uint64_t DecrBy(std::string_view key);

	bool Ping();

protected:
	// @brief Redis 명령어 전송
	// @param command: Redis 명령어
	// @return 성공 여부
	// @note 응답 값이 필요하지 않을 때 사용
	bool SendCommondNoReply(std::string&& command);

	redisReply* SendCommand(std::string&& command);

private:
	makga::lib::SharedMutex mutex_;
	std::shared_ptr<redisContext> redis_context_;

	std::string host_;
	int port_;

	std::string id_;
	std::string password_;

	int db_id_;
};

template<typename ...Args>
bool RedisConnector::MSet(std::string_view key, std::string_view value, ...)
{
}
} // namespace makga::database