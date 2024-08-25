#pragma once

namespace core {
namespace network {
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

class IOContext
{
public:
	IOContext() = default;
	virtual ~IOContext() = default;
};
} // namespace network
} // namespace core