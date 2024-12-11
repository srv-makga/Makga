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

	virtual Id SessionId() const { return 0; }
};
} // namespace network
} // namespace core