module;

#include <Windows.h>
#include <memory>

export module makga.network.iocp.object;

import makga.network.iocp.event;

export namespace makga::network {
export class IocpObject : public std::enable_shared_from_this<IocpObject>
{
public:
	IocpObject() = default;
	virtual ~IocpObject() = default;

	virtual void Dispatch(IocpEvent* iocp_event, int bytes_transferred = 0) = 0;
	virtual HANDLE GetHandle() const = 0;
};
} // namespace makga::network