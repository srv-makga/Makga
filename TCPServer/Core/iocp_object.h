#pragma once

namespace core {
namespace network {

class IOCPEvent;

class IOCPObject : public std::enable_shared_from_this<IOCPObject>
{
public:
	IOCPObject() = default;
	virtual ~IOCPObject() = default;

	virtual void Dispatch(IOCPEvent* _iocp_event, int _bytes_transferred = 0) abstract;
};
} // namespace network
} // namespace core