#pragma once

#include "game_header.h"
#include "../Core/iocp_connector.h"

class AppConnector
{
public:
	AppConnector();
	virtual ~AppConnector();

	bool Initialize();
	void Finalize();

private:
	std::map<eServerType, std::shared_ptr<core::network::IocpConnector>> m_connectors;
};