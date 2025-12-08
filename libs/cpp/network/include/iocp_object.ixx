module;

#include <Windows.h>
#include <memory>

export module makga.network.iocp.object;

import makga.network.iocp.event;

namespace makga::network {
class IocpObject : public std::enable_shared_from_this<IocpObject>
{
public:
	IocpObject() = default;
	virtual ~IocpObject() = default;

	virtual void Dispatch(IocpEvent* _iocp_event, int _bytes_transferred = 0) = 0;
	virtual HANDLE GetHandle() const = 0;
};
} // namespace makga::network