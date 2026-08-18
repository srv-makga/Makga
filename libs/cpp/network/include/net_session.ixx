module;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#endif

export module makga.network.session;

import <atomic>;
import <condition_variable>;
import <cstdint>;
import <mutex>;

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

	// @brief 네트워크 세션의 연결/끊김/수신/송신 콜백 정의
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

	// 비동기 I/O 수명 관리: close 이후 신규 요청은 거부하고, 기존 요청이
	// 모두 completion 될 때까지 소유자가 drain을 기다릴 수 있다.
	bool TryBeginIo();
	void CompleteIo();
    // true이면 호출자가 Active→Closing 전이를 최초로 획득했다.
    bool TryBeginClose();
	void BeginClose();
	bool ResetIoState();
	bool IsClosing() const { return closing_.load(); }
	std::uint32_t PendingIoCount() const { return pending_io_.load(); }
	void WaitForIoDrain();

protected:
	Id id_;
	IPEndPoint ep_;

private:
	std::atomic_bool closing_{false};
	std::atomic<std::uint32_t> pending_io_{0};
	mutable std::mutex io_mutex_;
	mutable std::condition_variable io_cv_;
};
} // namespace makga::network