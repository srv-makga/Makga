#pragma once

#include "buffer_ring.hpp"
#include "object_pool.h"
#include <vector>
#include <memory>

namespace core {
namespace network {
class NetBuffer : public BufferRing<char>
{
public:
	NetBuffer(std::size_t _buffer_size)
		: BufferRing(m_data.data(), _buffer_size)
		, m_data(_buffer_size)
	{
	}

	NetBuffer() = delete;
	NetBuffer(const NetBuffer& other) = delete;
	NetBuffer(NetBuffer&& other) = delete;
	NetBuffer& operator=(const NetBuffer& other) = delete;
	NetBuffer& operator=(NetBuffer&& other) = delete;
	virtual ~NetBuffer() = default;

public: // static
	inline static std::shared_ptr<NetBuffer> Pop()
	{
		return s_pool.Pop();
	}

	inline static void InitPool(std::size_t _initial_size, std::function<NetBuffer*()> _create_func)
	{
		s_pool.Initialize(_initial_size, _create_func);
	}

	inline static void ClearPool()
	{
		s_pool.Finalize();
	}

	inline static core::SharedObjectPool<NetBuffer> s_pool;

private:
	std::vector<char> m_data;
};
} // namespace nentwork
} // namespace core