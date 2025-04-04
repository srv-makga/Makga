#pragma once

#include <memory>

namespace core {
namespace network {
class IocpEvent;

class IocpObject : public std::enable_shared_from_this<IocpObject>
{
public:
	IocpObject() = default;
	virtual ~IocpObject() = default;

	virtual void Dispatch(IocpEvent* _iocp_event, int _bytes_transferred = 0) = 0;
	virtual HANDLE GetHandle() const = 0;
};
} // namespace network
} // namespace core