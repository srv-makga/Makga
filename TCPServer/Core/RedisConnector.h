#pragma once
#include "hiredis.h"
#include <map>
#include <functional>

class redisReplyDeleter
{
public:
	redisReplyDeleter() {}
	virtual ~redisReplyDeleter() {}

public:
	template <typename redisReply>
	void operator()(redisReply* pReply)
	{
		if (nullptr != pReply)
			freeReplyObject(pReply);
	}

};

class RedisConnector
{
public:
	RedisConnector();
	virtual ~RedisConnector();

	int GetRID() { return m_RID; }

	bool Init(int id, char const* ipAddr, WORD const& port);

	bool Connect();
	bool Disconnect();
	bool PING();

public:
	template<typename ... ARGS>
	static std::string format_string(const std::string& _format, ARGS ... _args);

public:
	template<typename... ARGS>
	std::shared_ptr<redisReply> redisCommand(char const* _format, const ARGS&... _args);
	std::shared_ptr<redisReply> redisCommandArgv(int _argc, const char** _argv, const size_t* _argvlen);

public:
	// DB º±≈√ (0~15)
	bool SELECT(int32_t _dbNum);

	// Hashes .....
	template<typename... Args>
	bool HSET(const std::string& _format, Args ... _args);
	bool HSET(char const* _key, char const* _field, char const* _value);
	bool HSET(char const* _key, char const* _field, __int64 const& _value);
	bool HDEL(char const* _key, char const* _field);
	bool HGET(char const* _key, char const* _field, std::string& _out);
	bool HGET(char const* _key, char const* _field, __int64& _out);
	bool HGETALL(char const* _key, std::map<std::string, std::string>& _mapOut);
	bool HGETALL(char const* _key, std::function<bool(const char*, const char*)> _callback);

	// sorted set (zset)
	bool ZADD(char const * _key, uint64_t _score, char const* _field);
	bool ZINCRBY(char const * _key, uint64_t _score, char const* _field);	
	bool ZREVRANK(char const * _key, char const* _field, uint32_t& _out);
	bool ZREVRANK(char const* _key, char const* _field, std::string& _out);
	bool ZREVRANGE(char const* _key, std::function<bool(std::shared_ptr<redisReply>)> _callback);
	bool ZREVRANGE(char const* _key, std::shared_ptr<redisReply> & _out);
	bool ZCOUNT(char const * _key, uint32_t& _out);
	bool ZREM(char const * _key, char const* _field);

private:
	int					m_RID = 0;
	std::string			m_IpAddr;
	WORD				m_Port = 0;
	redisContext*		m_redisContext = nullptr;
};

template<typename ... ARGS>
std::string RedisConnector::format_string(const std::string& _format, ARGS ... _args)
{
	char buffer[1024];
	size_t size = snprintf(nullptr, 0, _format.c_str(), _args ...) + 1;
	if (size > 1024)
		return std::string();
	snprintf(buffer, 1024, _format.c_str(), _args ...);
	return std::string(buffer, buffer + size - 1);
}

template<typename ... ARGS>
std::shared_ptr<redisReply> RedisConnector::redisCommand(char const* _format, const ARGS&... _args)
{
	if (nullptr == m_redisContext)
	{
		if (false == this->Connect())
		{
			LOG_ERROR << "Redis Command Connect Failed- rid:" << m_RID;
			return nullptr;
		}
	}

	TICK_TIME StartTime = GET_CURR_TICKTIME;
	std::shared_ptr<redisReply> Reply((redisReply*)::redisCommand(m_redisContext, _format, _args...), redisReplyDeleter());
	TICK_TIME EndTime = GET_CURR_TICKTIME;

	if (EndTime - StartTime > 100)
	{
		LOG_ERROR <<"Redis SlowQuery Time - rid:" << m_RID << ", time:" << EndTime - StartTime;
	}

	return Reply;
}

template<typename ... ARGS>
bool RedisConnector::HSET(const std::string& _format, ARGS ... _args)
{
	std::shared_ptr<redisReply> reply = redisCommand((std::string("HSET ") + format_string(_format, _args ...)).c_str());

	if (nullptr == reply)
		return false;

	if (nullptr != reply->str)
		return false;

	return true;
}
