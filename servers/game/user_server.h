#pragma once

#include "../common/iocp_server.h"

class UserServer : public IocpServer
{
public:
	UserServer(makga::network::IPEndPoint ep, std::shared_ptr<makga::network::IocpCore> core);
	virtual ~UserServer();

protected:
	void CreateSession(std::size_t max_connect_count) override;
};