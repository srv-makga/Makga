#pragma once

#include "core_header.h"

namespace core {
namespace network {
class NetCore
{
public:
	enum CoreType
	{
		IOCP,
		RIO
	};

	NetCore() = default;
	NetCore(const NetCore&) = delete;
	NetCore(NetCore&&) = delete;
	NetCore& operator=(const NetCore&) = delete;
	NetCore& operator=(NetCore&&) = delete;
	virtual ~NetCore() = default;

	virtual CoreType GetCoreType() const = 0;
};
} // namespace network
} // namespace core