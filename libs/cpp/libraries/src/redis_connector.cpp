#include <memory>
#include <optional>
#include <stdexcept>
#include <winsock2.h>
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
}
} // makga::lib::database