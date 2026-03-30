module;

#include <memory>
#include <vector>
#include <thread>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif


export module makga.network.rio.acceptor;
import makga.network.acceptor;
import makga.network.rio.service;

namespace makga::network {
export class RioAcceptor : public NetAcceptor {
public:
	RioAcceptor(std::shared_ptr<RioService> service);
	virtual ~RioAcceptor();

	bool Start() override;
	void Stop() override;

	bool CanAcceptSession() const override;
	std::size_t GetConnectCount() const override;
	std::size_t GetMaxConnectCount() const override;

	void RegisterAccept();
	void ProcessAccept(SOCKET client_socket);

private:
	void AcceptThread();

private:
	std::shared_ptr<RioService> service_;
	SOCKET listen_socket_;
	std::atomic<bool> is_running_;
	std::thread accept_thread_;
};
} // namespace makga::network