#pragma once

#include "../Common/packet.h"
#include "../Core/net_buffer.h"
#include "user.h"

class PacketPool : public core::ObjectPool<std::shared_ptr<Packet>>, public core::pattern::Singleton<PacketPool> {};
class BufferPool : public core::ObjectPool<std::shared_ptr<core::network::NetBuffer>>, public core::pattern::Singleton<BufferPool> {};

void InitPool()
{
	PacketPool::Instance().Initialize(CONFIG.pool_packet_init_count, 10);
	BufferPool::Instance().Initialize(CONFIG.pool_packet_init_count, 10, CONFIG.buffer_size_write);
}