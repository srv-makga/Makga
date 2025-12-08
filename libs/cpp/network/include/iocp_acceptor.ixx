module;

#include <windows.h>

export module makga.network.service.iocp.acceptor;

import makga.network.iocp.service;
import makga.network.iocp.object;

export namespace makga::network {
class IocpAcceptor : public IocpObject
{
public:
	IocpAcceptor();
	virtual ~IocpAcceptor();

	bool Initialize();
	void Finalize();
};
} // namespace makga::network