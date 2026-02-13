module;

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <Windows.h>
// windows.h removed from module interface to avoid header conflicts
//#pragma comment(lib, "ws2.lib")
//#pragma comment(lib, "mswsock.lib")
#endif
#include <cstdint>

export module makga.network.session;

import makga.network.socket;
import makga.network.service;
import makga.network.endpoint;

namespace makga::network {
export class NetSession
{
public:
	using Id = uint64_t;

	NetSession()
		: id_(0)
		, ep_()
	{
	}
	virtual ~NetSession() = default;

	// @brief �񵿱� ��û�� ����� ȣ��
	virtual void OnConnect() = 0;
	virtual void OnDisconnect() = 0;
	virtual void OnRecv(int bytes_transferred) = 0;
	virtual void OnSend(int bytes_transferred) = 0;

	virtual SOCKET GetSocket() const = 0;
	virtual char* GetWritePosition() = 0;

	Id GetSessionId() const;
	void SetSessionId(Id id);

	const IPEndPoint& GetEndPoint() const;
	void SetEndPoint(SOCKADDR_IN addr);
	void SetEndPoint(makga::network::IPEndPoint ep);

protected:
	Id id_;
	IPEndPoint ep_;
};
} // namespace makga::network