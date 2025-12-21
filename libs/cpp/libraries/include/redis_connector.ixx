#include <memory>
#include <optional>
#include <stdexcept>
#include <sstream>
#include <type_traits>

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
	bool Set(std::string_view key, std::string_view value);
	bool Del(std::string_view key);

	bool SetEx(std::string_view key, std::string_view value, std::chrono::seconds ttl = std::chrono::seconds::zero()); // 만료 시간과 함께 설정

	bool Exists(std::string_view key);
	void Unlink(std::string_view key);

	template<typename... Args>
	bool MSet(std::string_view key, std::string_view value, Args&&... args);
	std::vector<std::string> MGet(const std::vector<std::string>& keys);

	uint64_t Incr(std::string_view key);
	uint64_t IncrBy(std::string_view key, uint64_t value);
	uint64_t Decr(std::string_view key);
	uint64_t DecrBy(std::string_view key, uint64_t value);

	// List
	template<typename... Args>
	int64_t LPush(std::string_view key, Args&&... args);

	template<typename... Args>
	int64_t RPush(std::string_view key, Args&&... args);

	std::optional<std::string> LPop(std::string_view key);
	std::optional<std::string> RPop(std::string_view key);


	bool Ping();

protected:
	// @brief Redis 명령어 전송
	// @param command: Redis 명령어
	// @return reply의 nullptr 여부
	// @note 응답 값이 필요하지 않을 때 사용
	bool SendCommandNoReply(std::string&& command);
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
bool RedisConnector::MSet(std::string_view key, std::string_view value, Args&&... args)
{
	static_assert((std::is_convertible_v<Args, std::string_view> && ...),
		"All additional arguments must be convertible to std::string_view");
	static_assert(sizeof...(Args) % 2 == 0, "MSet requires an even number of additional arguments (key,value pairs)");

	std::ostringstream command;
	command << "MSET " << key << " " << value;

	((command << " " << std::string_view(args)), ...);

	return SendCommandNoReply(command.str());
}

template<typename... Args>
int64_t RedisConnector::LPush(std::string_view key, Args&&... args)
{
	std::ostringstream command;
	command << "LPUSH " << key;
	((command << " " << args), ...);

	UniqueRedisReply reply(SendCommand(command.str()));
	if (nullptr == reply)
	{
		return -1;
	}

	if (REDIS_REPLY_INTEGER != reply->type)
	{
		return -1;
	}

	return static_cast<int64_t>(reply->integer);
}

template<typename... Args>
int64_t RedisConnector::RPush(std::string_view key, Args&&... args)
{
	std::ostringstream command;
	command << "RPUSH " << key;
	((command << " " << args), ...);

	UniqueRedisReply reply(SendCommand(command.str()));
	if (nullptr == reply)
	{
		return -1;
	}

	if (REDIS_REPLY_INTEGER != reply->type)
	{
		return -1;
	}

	return static_cast<int64_t>(reply->integer);
}


} // namespace makga::database