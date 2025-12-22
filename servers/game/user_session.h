#pragma once

import makga.network.iocp.session;

class UserSession : public makga::network::IocpSession
{
public:
	UserSession();
	virtual ~UserSession();

	bool Initialize();
	void Finalize();

	void ProcConnect() override;
	void ProcDisconnect() override;
	std::size_t ProcRecv(char* data, std::size_t bytes_transferred) override;
	void ProcSend(std::int32_t bytes_transferred) override;
};