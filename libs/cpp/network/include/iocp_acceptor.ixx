module;

#include "WinSock2.h"
#include "Windows.h"
#include <memory>
#include <mutex>

export module makga.network.iocp.acceptor;

import <vector>;
import <queue>;
import makga.network.iocp.service;
import makga.network.iocp.object;
import makga.network.iocp.event;

export namespace makga::network {
export class IocpAcceptor : public IocpObject
{
public:
	IocpAcceptor(std::shared_ptr<IocpService> server);
	IocpAcceptor() = delete;
	IocpAcceptor(const IocpAcceptor& _other) = delete;
	IocpAcceptor(IocpAcceptor&& _other) = delete;
	IocpAcceptor& operator= (const IocpAcceptor& _other) = delete;
	IocpAcceptor& operator= (IocpAcceptor&& _other) = delete;
	virtual ~IocpAcceptor();

	bool Initialize();
	void Finalize();

	bool Start();
	void Stop();
	void CloseSocket();

	bool RegisterAccept(IocpAcceptEvent* event);
	void ProcessAccept(IocpAcceptEvent* event);

	bool CanAcceptSession() const;

	std::size_t GetConnectCount() const;
	std::size_t GetMaxConnectCount() const;

public: // IocpObject
	HANDLE GetHandle() const override;
	void Dispatch(IocpEvent* iocp_event, int bytes_transferred = 0) override;

protected:
	void PushFreeAcceptEvent(IocpAcceptEvent* _event);
	IocpAcceptEvent* PopFreeAcceptEvent();

private:
	std::shared_ptr<IocpService> service_;
	SOCKET socket_;

	mutable std::mutex mutex_;
	std::vector<IocpAcceptEvent*> accept_events_;
	std::queue<IocpAcceptEvent*> free_accept_events_;
};
} // namespace makga::network