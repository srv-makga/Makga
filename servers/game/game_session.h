#pragma once

#include "../common/dispatcher.h"
#include "../common/iocp_session.h"

import makga.network.iocp.event;
import makga.network.iocp.service;

class GameSession : public IocpSession
{
public:
	// @todo 생성자에 dispatcher나 handler 넣고 처리, 세션 타입 추가
	GameSession(SessionType _type, std::shared_ptr<makga::network::IocpService> service);
	virtual ~GameSession();

	bool Initialize();
	void Finalize();

public: // IocpSession
	void ProcConnect() override;
	void ProcDisconnect() override;
	std::size_t ProcRecv(char* data, std::size_t bytes_transferred) override;
	void ProcSend(int32_t bytes_transferred) override;

protected:
	SessionType session_type_;
};