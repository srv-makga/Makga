#pragma once
#include "core_header.h"

namespace core {
namespace network {
class Session
{
public:
	using Id = uint64_t;

public:
	Session() = default;
	virtual ~Session() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;

	virtual Id GetSessionId() const = 0;
	virtual void SetSessionId(Id _id) = 0;
};
} // namespace network
} // namespace core