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
		Packet::InitPool(
			m_config->pool_packet_init_count,
			[buffer_size = m_config->buffer_size_read]() { return new Packet(false); }
		);
		NetBuffer::InitPool(
			m_config->pool_packet_init_count,
			[buffer_size = m_config->buffer_size_read]() { return new NetBuffer(buffer_size); }
		);
		job.Initialize(m_config->pool_packet_init_count, 10, []() { return std::make_shared<Job>(); }, [](std::shared_ptr<Job> _object) { _object.reset(); });
	}

public:
	core::ObjectPool<std::shared_ptr<Job>> job;

	std::shared_ptr<AppConfig> m_config;
};

#define POOL	Pool::Instance()