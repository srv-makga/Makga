#pragma once

#include "config_game.h"
#include "../Common/server_base.h"
#include "../Common/acceptor_base.h"
#include "../Common/connector_base.h"
#include "../Common/manager_base.hpp"

struct ConfigGame;

class ServerGame : public ServerBase
{
public:
	ServerGame() = delete;
	ServerGame(ConfigGame* _cconfig);
	virtual ~ServerGame();

	void Initialize();
	void Finalize();

	bool StartUp() override;
	bool StartEnd() override;

	bool InitConnector();
	bool InitAcceptor();

public:
	ConnectorBase* Connector(fb::eServerType _server_type) const;
	AcceptorBase* Acceptor(fb::eServerType _server_type) const;

private:
	JobHandler* m_job_handler;
	NetHandler* m_net_handler;

	std::vector<fb::eServerType> m_connect_list;
	ManagerBase<fb::eServerType, ConnectorBase*> m_connectors;

	std::vector<fb::eServerType> m_accept_list;
	ManagerBase<fb::eServerType, AcceptorBase*> m_acceptors;

	std::thread m_scheduler;
};
