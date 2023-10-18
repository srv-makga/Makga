#pragma once

#include "hiredis.h"
#include <string>

namespace core {
namespace db {
namespace redis {
class RedisReply
{
public:
	RedisReply(void* _reply); // redisReply
	virtual ~RedisReply();

	bool IsValid() const;

	bool IsString() const;
	bool IsArray() const;
	bool IsInt() const;

	template<typename T>
	T Get() const;

	template<>
	bool Get<bool>() const;

	template<>
	int32_t Get<int32_t>() const;

	template<>
	uint32_t Get<uint32_t>() const;

	template<>
	int64_t Get<int64_t>() const;

	template<>
	uint64_t Get<uint64_t>() const;

	template<>
	std::string Get<std::string>() const;

private:
	redisReply* m_reply;
};

template<typename T>
inline T RedisReply::Get() const
{
	return T();
}

template<>
inline bool RedisReply::Get<bool>() const
{
	if (false == IsInt())
	{
		return false;
	}

	return 0 < m_reply->integer;
}

template<>
inline int32_t RedisReply::Get<int32_t>() const
{
	if (false == IsInt())
	{
		return 0;
	}

	return static_cast<int32_t>(m_reply->integer);
}

template<>
inline uint32_t RedisReply::Get<uint32_t>() const
{
	if (false == IsInt())
	{
		return 0;
	}

	return static_cast<uint32_t>(m_reply->integer);
}

template<>
inline int64_t RedisReply::Get<int64_t>() const
{
	if (false == IsInt())
	{
		return 0;
	}

	return static_cast<int64_t>(m_reply->integer);
}

template<>
inline uint64_t RedisReply::Get<uint64_t>() const
{
	if (false == IsInt())
	{
		return 0;
	}

	return static_cast<uint64_t>(m_reply->integer);
}

template<>
inline std::string RedisReply::Get<std::string>() const
{
	if (false == IsString())
	{
		return std::string();
	}

	return std::string(m_reply->str);
}
} // namespace redis
} // namespace db
} // namespace core