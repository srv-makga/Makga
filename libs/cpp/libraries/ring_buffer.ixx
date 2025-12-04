module;

#include <iostream>;

export module makga.lib.ring_buffer;

export namespace makga::lib {
export template<typename T>
class RingBuffer
{
public:
	RingBuffer(std::size_t buffer_size)
	{

	}

	virtual ~RingBuffer()
	{

	}

protected:
	T* m_buffer;
	std::size_t m_buffer_size;
	std::size_t m_write_offset;
	std::size_t m_read_offset;
};
} // namespace makga::lib