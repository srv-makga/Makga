#pragma once
#include "RedisConnector.h"
#include "T3Synchronized.h"
#include "T3SingleTon.h"

class RedisManager : public SingleTon<RedisManager>
{
public:
	RedisManager();
	virtual ~RedisManager();

	bool Init( OBJECT_ID _redisConID, char const* _ipAddr, WORD _port);
	std::shared_ptr<RedisConnector> GetRedisConnection(OBJECT_ID _conID);
	bool Disconnect(OBJECT_ID _conID);
	bool DisConnectAll();

	bool OnTimer();

private:
	std::shared_mutex m_Lock;
	std::map<OBJECT_ID, std::shared_ptr<RedisConnector>> m_mapRedisCons;
};

#define REDIS_MGR RedisManager::getInstance()