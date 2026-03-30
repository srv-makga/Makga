module;

#include <memory>
#include <vector>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif

export module makga.network.rio.session;
import makga.network.session;
import makga.network.rio.core;
import makga.network.rio.event;
import makga.network.endpoint;
import makga.network.rio.service;

namespace makga::network {
export class RioSession : public NetSession
{
public:
	RioSession();
	virtual ~RioSession();

	bool Initialize(RioService* service);
	void Finalize();

	virtual void OnRecv(int bytes_transferred) = 0;
	virtual void OnSend(int bytes_transferred) = 0;
	virtual void OnDisconnect() = 0;

	SOCKET GetSocket() const override { return socket_; }
	void SetSocket(SOCKET socket) { socket_ = socket; }

	char* GetWritePosition() override; // @todo Implement if needed buffer management

	bool PostRecv();
	bool PostSend(char* buffer, int length);

private:
	RioService* service_;
	SOCKET socket_;
	RIO_RQ request_queue_;

	// Simple single buffer for demo purposes, real implementation needs ring
	// buffer or similar
	RIO_BUFFERID buffer_id_;
	char* recv_buffer_;
	char* send_buffer_;

	RioRecvEvent recv_event_;
	RioSendEvent send_event_;
};
} // namespace makga::network