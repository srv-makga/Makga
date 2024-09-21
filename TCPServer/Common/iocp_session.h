#pragma once

#include "session_base.h"
#include "../Core/socket_tcp.h"
#include "../Core/io_context.hpp"
#include "../Core/buffer_ring.hpp"

// @todo IOCPContext ÇÊ¿ä

class IOCPSession : public SessionBase
{
public:
	using Socket_t = core::network::SocketTcp;
	using Buffer_t = core::BufferRing<char>;
	using Context_t = std::shared_ptr<core::network::IOContext<8192>>;

public:
	IOCPSession();
	virtual ~IOCPSession();

	bool Initialize();
	void Finalize();

public: // JobOwner
	bool ProcPacket(NetPacket* _packet) override;
	ThreadId_t ThreadId() const override;

public: // SessionBase
	bool RecvPacket(NetPacket* packet) override;
	void OnError(const std::string& _errmsg) override;
	void OnError(std::exception& _exception) override;

private:
	Socket_t m_socket;

	std::atomic<bool> m_is_connected;

	std::size_t m_pending_bytes;
	std::size_t m_sending_bytes;
	std::size_t m_sent_bytes;
	std::size_t m_received_bytes;

	Time_t m_accept_time;
	Time_t m_disconnect_time;
	Time_t m_receive_time;
	Time_t m_send_time;
};