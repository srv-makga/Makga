#pragma once

namespace core {
namespace network {
class IocpEvent;

class IocpObject
{
public:
	IocpObject() = default;
	virtual ~IocpObject() = default;

	virtual void Dispatch(IocpEvent* _iocp_event, int _bytes_transferred = 0) abstract;
};
} // namespace network
} // namespace core