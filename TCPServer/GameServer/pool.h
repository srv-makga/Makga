#pragma once

#include "../Core/net_buffer.h"
#include "../Common/packet.h"
#include "../Common/job.h"
#include "../Core/singleton.hpp"
#include "user.h"

class Pool : public core::pattern::Singleton<Pool>
{
public:
	Pool(std::shared_ptr<AppConfig> _config)
		: m_config(_config)
	{
	}

	~Pool()
	{
		m_config = nullptr;
	}

	void Initialize()
	{
		Packet::InitPool(m_config->pool_packet_init_count, 10, [buffer_size = m_config->buffer_size_read]() { return std::make_shared<Packet>(new Packet(false), [](Packet* _p) { std::shared_ptr<Packet> new_packet(_p); Packet::Push(new_packet); }); }, [](std::shared_ptr<Packet> _object) { _object.reset(); });
		buffer.Initialize(m_config->pool_packet_init_count, 10, [buffer_size = m_config->buffer_size_read]() { return std::make_shared<core::network::NetBuffer>(buffer_size); }, [](std::shared_ptr<core::network::NetBuffer> _object) { _object.reset(); });
		job.Initialize(m_config->pool_packet_init_count, 10, []() { return std::make_shared<Job>(); }, [](std::shared_ptr<Job> _object) { _object.reset(); });
	}

public:
	core::ObjectPool<std::shared_ptr<core::network::NetBuffer>> buffer;
	core::ObjectPool<std::shared_ptr<Job>> job;

	std::shared_ptr<AppConfig> m_config;
};

#define POOL	Pool::Instance()