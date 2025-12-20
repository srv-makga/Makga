#include <memory>
#include <optional>
#include <stdexcept>
#include <winsock2.h>
#include <chrono>
#include "../../3rdparty/hiredis/hiredis.h"

module makga.lib.database.redis.connector;

import <string>;
import <chrono>;
import <format>;
import makga.lib.lock;
import makga.lib.logger;

namespace makga::lib::database {
RedisConnector::RedisConnector(std::string_view host, int port)
	: redis_context_(nullptr)
	, host_(host)
	, port_(port)
	, db_id_(0)
{
}

RedisConnector::~RedisConnector() noexcept
{
	Disconnect();
}

bool RedisConnector::Connect()
{
	if (true == host_.empty() || 0 == port_)
	{
		MakgaLogger::Error("Redis URI is empty.");
		return false;
	}

	timeval timeout = { 1, 500000 };
	redis_context_.reset(::redisConnectWithTimeout(host_.c_str(), port_, timeout));

	if (nullptr == redis_context_ || 0 != redis_context_->err)
	{
		MakgaLogger::Error(std::format("Redis connection failed:{0}", (redis_context_ ? redis_context_->errstr : "unknown error")));
		redis_context_.reset();
		return false;
	}

	std::shared_ptr<redisReply> reply(static_cast<redisReply*>(::redisCommand(redis_context_.get(), "PING")), ::freeReplyObject);
}

void RedisConnector::Disconnect() noexcept
{
	redis_context_.reset();
}

bool RedisConnector::IsConnected() const
{
	return nullptr != redis_context_;
}

std::optional<std::string> database::RedisConnector::Get(std::string_view key) 
{
	UniqueRedisReply reply(SendCommand(std::format("GET {0}", key.data())));
	if (nullptr == reply)
	{
		return std::nullopt;
	}

	if (REDIS_REPLY_STRING != reply->type)
	{
		return std::nullopt;
	}

	return std::string(reply->str, reply->len);
}

[[nodiscard]]
redisReply* RedisConnector::SendCommand(std::string&& command)
{
	if (true == command.empty())
	{
		return nullptr;
	}

	auto context = redis_context_;

	if (false == IsConnected())
	{
		return nullptr;
	}

	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
	redisReply* reply = reinterpret_cast<redisReply*>(::redisCommand(redis_context_.get(), command.c_str()));
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();

	auto taken_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	if (100 < taken_time)
	{
		MakgaLogger::Warn(std::format("Redis command took too long: {0} ms, command: {1}", taken_time, command));
	}

	if (nullptr == reply)
	{
		MakgaLogger::Error(std::format("Redis command failed: {0}", command));
		return nullptr;
	}

	return reply;
}
} // makga::lib::database