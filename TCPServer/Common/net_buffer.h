#pragma once

#include "../Core/buffer_ring.hpp"

class NetBuffer : public core::BufferRing<char>
{
public:
	NetBuffer(std::size_t _length);
	NetBuffer(const NetBuffer&) = delete;
	NetBuffer(NetBuffer&&) = delete;
	NetBuffer& operator=(const NetBuffer&) = delete;
	NetBuffer& operator=(NetBuffer&&) = delete;
	virtual ~NetBuffer();

private:
	std::vector<char> m_data;
};