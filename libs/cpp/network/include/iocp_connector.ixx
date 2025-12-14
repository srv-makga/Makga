module;

#include "WinSock2.h"
#include "Windows.h"
#include <memory>

export module makga.network.iocp.connector;

import <vector>;
import <queue>;
import makga.network.iocp.service;
import makga.network.iocp.object;
import makga.network.iocp.event;
import makga.lib.lock;

export namespace makga::network {
export class IocpConnector : public IocpObject
{
public:
	IocpConnector(std::shared_ptr<IocpService> client);
	IocpConnector() = delete;
	IocpConnector(const IocpConnector&) = delete;
	IocpConnector(IocpConnector&&) = delete;
	IocpConnector& operator= (const IocpConnector&) = delete;
	IocpConnector& operator= (IocpConnector&&) = delete;
	virtual ~IocpConnector();

	bool Initialize();
	void Finalize();

	bool Start();
	void Stop();

	bool RegisterConnect(IocpAcceptEvent* event);
	void ProcessConnect(IocpAcceptEvent* event);

public: // IocpObject
	HANDLE GetHandle() const override;
	void Dispatch(IocpEvent* event, int bytes_transferred = 0) override;

protected:
	std::shared_ptr<IocpService> service_;
	SOCKET socket_;

	std::shared_ptr<IocpSession> session_;
};
} // namespace makga::network