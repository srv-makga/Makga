#pragma once

#include "../Core/net_buffer.h"
#include "../Common/packet.h"
#include "../Common/job.h"
#include "../Core/singleton.hpp"
#include "user.h"

class Pool : public core::pattern::Singleton<Pool>
{
public:
	Pool() = default;
	~Pool() = default;

	void Initialize()
	{
		packet.Initialize(CONFIG.pool_packet_init_count, 10, []() { return std::make_shared<Packet>(); }, [](std::shared_ptr<Packet> _object) { _object.reset(); });
		buffer.Initialize(CONFIG.pool_packet_init_count, 10, []() { return std::make_shared<core::network::NetBuffer>(CONFIG.buffer_size_user); }, [](std::shared_ptr<core::network::NetBuffer> _object) { _object.reset(); });
		job.Initialize(CONFIG.pool_packet_init_count, 10, []() { return std::make_shared<Job>(); }, [](std::shared_ptr<Job> _object) { _object.reset(); });
	}

public:
	core::ObjectPool<std::shared_ptr<Packet>> packet;
	core::ObjectPool<std::shared_ptr<core::network::NetBuffer>> buffer;
	core::ObjectPool<std::shared_ptr<Job>> job;
};

#define POOL	Pool::Instance()