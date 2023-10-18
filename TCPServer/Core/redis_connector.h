#pragma once
#include "hiredis.h"
#include "db_connector.h"
#include "redis_reply.h"
#include <sstream>

namespace core {
namespace db {
namespace redis {
class RedisConnector
{
public:
	RedisConnector();
	virtual ~RedisConnector();

	bool Init(const char* ipAddr, uint16_t _port);

	bool Connect();
	bool Disconnect();

	bool PING();

	// Common
	bool Exists(const char* _key);
	void Unlink(const char* _key);
	std::shared_ptr<RedisReply> Keys(const char* _partten);
	bool Rename(const char* _old, const char* _new);
	bool Expire(const char* _key, std::time_t _seconds);
	std::string Dump(const char* _key);
	bool Migrate(const char* _host, int _port, const char* _key, std::time_t _timeout, bool _is_copy, bool _is_replace); // _timeout ms

	// Strings
	bool Set(const char* _key, const char* _value);
	int32_t Del(const char* _key); // key는 복수가 가능하다
	std::string Get(const char* _key);
	int64_t Incr(const char* _key); // 1씩 증가
	int64_t Decr(const char* _key); // 1씩 감소
	int64_t IncrBy(const char* _key, int64_t _n); // n씩 증가
	int64_t DecrBy(const char* _key, int64_t _n); // n씩 감소
	template<typename ...Args>
	bool MSet(const char* _key1, Args... _args);
	template<typename ...Args>
	std::shared_ptr<RedisReply> MGet(Args... _keys);
	int64_t Append(const char* _key, const char* _value); // return은 글자 길이

	// Lists
	template<typename ...Args>
	int64_t LPush(const char* _key, Args... _args);
	// 
	// Sets
	// ZSets
	// Hashes

protected:
	template<typename ...Args>
	std::string Format(Args... _args);

	std::string m_ip;
	int32_t m_port = 0;
	redisContext* m_context = nullptr;

	std::ostringstream	m_oss;
};

template<typename ...Args>
bool RedisConnector::MSet(const char* _key, Args... _args)
{
	RedisReply reply(::redisCommand(m_context, Format("MSET", _key, _args...).c_str()));

	return reply.Get<int64_t>();
}

template<typename ...Args>
std::shared_ptr<RedisReply> RedisConnector::MGet(Args... _keys)
{
	return std::shared_ptr<RedisReply>(::redisCommand(m_context, Format("MGET", _keys...).c_str()));
}

template<typename ...Args>
int64_t RedisConnector::LPush(const char* _key, Args... _args)
{
	RedisReply reply(::redisCommand(m_context, Format("LPUSH", _key, _args...).c_str()));

	return reply.Get<int64_t>();
}

template<typename ...Args>
std::string RedisConnector::Format(Args... _args)
{
	m_oss.str("");
	((m_oss << " " << _args), ...);
	return m_oss.str();
}

} // namespace redis
} // namespace db
} // namespace core