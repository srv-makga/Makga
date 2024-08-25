#pragma once

#include "io_context.h"

class ISession;

namespace core {
namespace network {
class IOCPContext : public IOContext
{
public:
	IOCPContext() = default;
	virtual ~IOCPContext() = default;

public:
	void Initialize()
	{
		m_type = IOType::None;
		m_session = nullptr;
	}

	void Finalize() {}

	void Set(IOType _type, ISession* _session)
	{
		m_type = _type;
		m_session = _session;
	}

public:
	IOType m_type;
	ISession* m_session;
};
} // namespace network
} // namespace core