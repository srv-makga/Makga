#pragma once

#include "buffer_pull.h"
#include "object_pool.h"

namespace core {
namespace buffer {
class BufferPool
{
public:
	BufferPool();
	~BufferPool();

	BufferPull* Rental(std::size_t _size);
	 void Return(BufferPull* _buffer);

private:
	std::vector<std::pair<std::size_t, ObjectPool<BufferPull*>>> m_list;
};
} // namespace buffer
} // namespace core