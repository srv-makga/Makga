#pragma once

import makga.network.iocp.event;
import makga.network.iocp.service;
import makga.network.iocp.session;

class Session : public makga::network::IocpSession
{
public:
	// @todo 생성자에 dispatcher나 handler 넣고 처리, 세션 타입 추가
	Session(SessionType _type, std::shared_ptr<makga::network::IocpService> service);
	virtual ~Session();

	bool Initialize();
	void Finalize();

public: // IocpSession
	void ProcConnect() override;
	void ProcDisconnect() override;
	std::size_t ProcRecv(char* data, std::size_t bytes_transferred) override;
	void ProcSend(std::int32_t bytes_transferred) override;

protected:
	SessionType session_type_;
};