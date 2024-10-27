#pragma once

#include "buffer_ring.hpp"

namespace core {
namespace network {
class RioBuffer : public BufferRing<char>
{
public:
	RioBuffer(std::size_t _buffer_size);
	RioBuffer() = delete;
	RioBuffer(const RioBuffer& other) = delete;
	RioBuffer(RioBuffer&& other) = delete;
	RioBuffer& operator=(const RioBuffer& other) = delete;
	RioBuffer& operator=(RioBuffer&& other) = delete;
	~RioBuffer();

	// send data
	int GetChunkSendSize();
	int GetSendDataSize();
	std::size_t GetSendOffset() { return m_send_pos; }

	bool OnSendBuffer(int sendSize);

private:
	void AllocateBuffer();

	// 데이터를 어디까지 전송했는지 체크하기 위함

	// ----tail===send===head---- //
	// ====head----tail====send== //

	std::size_t m_send_pos;
};
} // namespace network
} // namespace core