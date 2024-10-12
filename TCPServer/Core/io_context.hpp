#pragma once

#include "net_header.h"
#include "buffer_pull.h"

class SessionBase;

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

class IOContext : public OVERLAPPED
{
public:
	IOContext()
		: m_type(IOType::None)
		, m_session(nullptr)
		, m_buffer(nullptr)
	{
		::memset((OVERLAPPED*)this, 0, sizeof(OVERLAPPED));
	}

	void Initialize()
	{
		m_type = IOType::None;
		m_session = nullptr;
		m_buffer->Initialize();
	}

	void SetSession(IOType _type, std::shared_ptr<SessionBase> _session)
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

		return m_buffer->Data();
	}

	std::size_t Size() const
	{
		return m_buffer ? m_buffer->Size() : 0;
	}

	IOType m_type;
	std::shared_ptr<SessionBase> m_session;
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