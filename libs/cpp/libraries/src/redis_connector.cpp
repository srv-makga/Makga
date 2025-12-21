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

	if (false == Ping())
	{
		MakgaLogger::Error("Redis PING failed.");
		redis_context_.reset();
		return false;
	}

	return true;
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

std::optional<std::string> database::RedisConnector::GetDel(std::string_view key)
{
	UniqueRedisReply reply(SendCommand(std::format("GETDEL {0}", key.data())));
	if (nullptr == reply)
	{
		return std::nullopt;
	}

	return std::optional<std::string>();
}

bool RedisConnector::Set(std::string_view key, std::string_view value, std::chrono::milliseconds ttl)
{
	bool ret = SendCommondNoReply(std::format("SET {0} {1}", key.data(), value.data()));

	if (std::chrono::milliseconds::zero() != ttl)
	{
		SendCommondNoReply(std::format("PEXPIRE {0} {1}", key.data(), ttl.count()));
	}

	return ret;
}

bool RedisConnector::Del(std::string_view key)
{
	return SendCommondNoReply(std::format("DEL {0}", key.data()));
}

bool RedisConnector::SendCommondNoReply(std::string&& command)
{
	UniqueRedisReply reply(SendCommand(std::move(command)));
	return nullptr != reply;
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

bool RedisConnector::Ping()
{
	if (false == IsConnected())
	{
		return false;
	}

	UniqueRepy reply(SendCommand("PING"));
	if (nullptr == reply)
	{
		return false;
	}

	if (REDIS_REPLY_STATUS != reply->type || 0 != std::string_view(reply->str, reply->len).compare("PONG"))
	{
		return false;
	}

	return true;
}
} // makga::lib::database