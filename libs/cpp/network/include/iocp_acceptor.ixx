module;

#include <memory>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif

export module makga.network.iocp.acceptor;

import <vector>;
import <queue>;
import makga.network.acceptor;
import makga.network.iocp.service;
import makga.network.iocp.event;
import makga.network.iocp.object;
import makga.lib.lock;

namespace makga::network {
export class IocpAcceptor : public IocpObject, public NetAcceptor
{
public:
	IocpAcceptor(std::shared_ptr<IocpService> server);
	IocpAcceptor() = delete;
	IocpAcceptor(const IocpAcceptor&) = delete;
	IocpAcceptor(IocpAcceptor&&) = delete;
	IocpAcceptor& operator= (const IocpAcceptor&) = delete;
	IocpAcceptor& operator= (IocpAcceptor&&) = delete;
	virtual ~IocpAcceptor();

	bool Initialize();
	void Finalize();

	bool Start() override;
	void Stop() override;

	bool RegisterAccept(IocpAcceptEvent* event);
	void ProcessAccept(IocpAcceptEvent* event);

	bool CanAcceptSession() const override;

	std::size_t GetConnectCount() const override;
	std::size_t GetMaxConnectCount() const override;

public: // IocpObject
	HANDLE GetHandle() const override;
	void Dispatch(IocpEvent* event, int bytes_transferred = 0) override;

protected:
	void PushFreeAcceptEvent(IocpAcceptEvent* event);
	IocpAcceptEvent* PopFreeAcceptEvent();

private:
	std::shared_ptr<IocpService> service_;
	SOCKET socket_;

	mutable makga::lib::Mutex mutex_;
	std::vector<IocpAcceptEvent*> accept_events_;
	std::queue<IocpAcceptEvent*> free_accept_events_;
};
} // namespace makga::network