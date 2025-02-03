#pragma once
#include "core_header.h"

namespace core {
namespace network {
class Session
{
public:
	using Id = uint64_t;
	enum Type
	{
		IOCPServer,
		IOCPClient,
		RIOServer,
		RIOClient,
	};

public:
	Session() = default;
	virtual ~Session() {}

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;

	virtual Id GetSessionId() const = 0;
	virtual void SetSessionId(Id _id) = 0;

	virtual Type GetSessionType() const = 0;
	virtual void SetSessionType(Type _type) = 0;
};
} // namespace network
} // namespace core