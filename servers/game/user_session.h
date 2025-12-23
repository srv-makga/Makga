#pragma once

import makga.network.iocp.event;
import makga.network.iocp.service;
import makga.network.iocp.session;

class UserSession : public makga::network::IocpSession
{
public:
	UserSession(std::shared_ptr<makga::network::IocpService> service);
	virtual ~UserSession();

	bool Initialize();
	void Finalize();

public: // IocpSession
	void ProcConnect() override;
	void ProcDisconnect() override;
	std::size_t ProcRecv(char* data, std::size_t bytes_transferred) override;
	void ProcSend(std::int32_t bytes_transferred) override;
};