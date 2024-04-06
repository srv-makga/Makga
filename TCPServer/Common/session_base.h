#pragma once

#include "common_type.h"
#include "net_actor.h"
#include "job_handler.h"

#include "../Core/socket_tcp.h"
#include "../Core/buffer_pull.h"
#include "../Core/io_context.hpp"
#include "../Core/object_pool.h"

// dispatcher 내용 추가

enum SessionCloseState
{
	CloseNone,
	CloseBegin,
	CloseEnd
};

class SessionBase : public JobOwner
{
public:
	using Buffer_t = core::BufferPull;

public:
	SessionBase(core::BufferFlag _flag, std::size_t _buffer_size);
	virtual ~SessionBase();

public:
	void Initialize();
	void Finallize();

	void Close();

	// @brief listen socket에 세션 소켓 accept 걸기
	// @parameter _socket listen socket
	bool PostAccept(SOCKET _socket);
	// @brief endpoint로 연결 시도
	// @parameter ip와 port 정보
	bool PostConnect(const core::network::EndPoint& _endpoint);
	bool PostDisconnect();
	bool PostZeroRecv();
	bool PostRecv();
	bool PostSend();

	void OnAccept(IOContext_t* _context);
	void OnConnect();
	void OnDisconnect();
	void OnRecv(DWORD _bytes_transferred, IOContext_t* _context);
	void OnSent(DWORD _bytes_transferred);

	// @brief send buffer에 데이터 추가
	// @detail 지연 전송을 위한
	bool PushData(char* _data, std::size_t _length);

	std::size_t Recv();
	// @brief send buffer에 있는 데이터 전송
	bool Send();
	
	virtual bool Send(PacketId_t _pid, fbb& _fbb);

	// @brief send buffer에 데이터 추가 후 전송
	virtual fb::eResult Send(char* data, std::size_t length);

	// @brief recv 버퍼에서 Packet 뽑아내기
	NetPacket* GetProcPacket();

	virtual bool RecvPacket(NetPacket* packet) = 0;
	virtual void OnError(const std::string& _errmsg) = 0;
	virtual void OnError(std::exception& _exception) = 0;

	bool IsConnect() const;
	bool IsSendable() const;

protected:
	bool IsSending() const;

	std::string RemoteAddress() const;
	void SetRemoteAddress(LPSOCKADDR _addr);
	void SetRemoteAddress(SOCKADDR_IN* _addr);
	void SetRemoteAddress(SOCKADDR_IN6* _addr);

public:
	SessionId_t SessionId() const;

	Socket_t RawSocket() const;

	Buffer_t* RecvBuffer() const;
	Buffer_t* SendBuffer() const;

	// @brief 수신 버퍼의 데이터 삽입 위치
	char* RecvPosition() const;
	// @brief 발신 버퍼의 데이터 삽입 위치
	char* SendPosition() const;

	std::time_t LastReceiveTime() const;
	void SetLastReceiveTime();

	std::time_t LastSendTime() const;
	void SetLastSendTime();

	NetActor* Actor() const; // acceptor, connector 가져오기
	void SetNetActor(NetActor* _actor);

protected:
	static std::atomic<SessionId_t> s_id;

private:
	SessionId_t m_session_id;

	core::network::SocketTcp m_socket;
	Buffer_t* m_recv_buffer;
	Buffer_t* m_send_buffer;

	std::time_t m_last_receive_time;	// 마지막으로 receive한 시간

	core::RWMutex m_send_mutex;
	int32_t m_send_count;				// send 호출한 횟수
	std::time_t m_last_send_time;		// 마지막으로 send한 시간

	SessionCloseState m_close_state;
	mutable core::CriticalSection m_cs_close_state;
	mutable core::CriticalSection m_cs_socket;

	// rio
	// RIO_RQ m_request_queue;

	NetActor* m_net_actor;

	LPFN_ACCEPTEX fn_acceptex = nullptr;
	LPFN_GETACCEPTEXSOCKADDRS fn_acceptex_sock_addr = nullptr;
	LPFN_CONNECTEX fn_connectex = nullptr;
};