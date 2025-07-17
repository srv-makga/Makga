#pragma once

#include "game_header.h"
#include "app_config.h"
#include "../Common/iocp_server.h"
#include "pool.h"

class AppServer : public Server_t
{
public:
	AppServer(std::shared_ptr<core::network::IocpCore> _core, const core::network::IPEndPoint& _endpoint);
	virtual ~AppServer();

public:
	bool Initialize() override;
	void Finalize() override;
};
