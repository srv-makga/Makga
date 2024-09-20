#pragma once

#include "net_header.h"
#include "object_pool.h"
#include "buffer_pull.h"

class ISession;

namespace core {
namespace network {
static constexpr std::size_t s_io_context_buffer_size = 1 << 13;

enum class IOType
{
	None,
	Send,
	Recv,
	Accept,
	Connect,
	Disconnect,
	Close,
};

template<std::size_t T = s_io_context_buffer_size>
class IOContext : public OVERLAPPED, public std::enable_shared_from_this<IOContext>, public core::ObjectPool<IOContext<T>*>
{
public:
	IOContext()
		: m_type(IOType::None)
		, m_session(nullptr)
		, m_buffer(new BufferPull(T))
	{
		::memset((OVERLAPPED*)this, 0, sizeof(OVERLAPPED));
	}

	void Initialize()
	{
		m_type = IOType::None;
		m_session = nullptr;
		m_buffer->Initialize();
	}
	void InitBuffer()
	{
		m_buffer->Initialize();
	}

	void SetSession(IOType _type, ISession* _session)
	{
		m_type = _type;
		m_session = _session;
	}

	char* Buffer() const
	{
		if (nullptr == m_buffer)
		{
			return nullptr;
		}

		return m_buffer->Buffer();
	}

	std::size_t Size() const
	{
		return m_buffer ? m_buffer->UsingSize() : 0;
	}

	std::size_t BufferSize() const
	{
		return T;
	}

	template<typename ...Args>
	static void InitPool(int _init_count, int _grow_count, Args... args)
	{
		core::ObjectPool<IOContext*>::Initialize(_init_count, _grow_count);
	}

	IOType m_type;
	ISession* m_session;
	BufferBase* m_buffer;
};

struct RIOContext : public RIO_BUF, public core::ObjectPool<RIOContext*>
{
	IOType m_type;
	SessionBase* m_session;

	RIOContext(IOType _type, SessionBase* _session);
};

} // namespace network
} // namespace core