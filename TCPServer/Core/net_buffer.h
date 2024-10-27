#pragma once

#include "buffer_ring.hpp"
#include <vector>

namespace core {
namespace network {
class NetBuffer : public BufferRing<char>
{
public:
	NetBuffer(std::size_t _buffer_size);

	NetBuffer() = delete;
	NetBuffer(const NetBuffer& other) = delete;
	NetBuffer(NetBuffer&& other) = delete;
	NetBuffer& operator=(const NetBuffer& other) = delete;
	NetBuffer& operator=(NetBuffer&& other) = delete;
	~NetBuffer();

private:
	std::vector<char> m_data;
};
} // namespace nentwork
} // namespace core