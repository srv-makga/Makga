#pragma once
#include "core_header.h"
#include "service.h"

namespace core {
namespace network {
class SessionInterface
{
public:
	using Id = uint64_t;

public:
	SessionInterface() = default;
	virtual ~SessionInterface() {}

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;

	virtual Id GetSessionId() const = 0;
	virtual void SetSessionId(Id _id) = 0;

	virtual ServiceType GetSessionType() const = 0;
	virtual void SetSessionType(ServiceType _type) = 0;
};
} // namespace network
} // namespace core