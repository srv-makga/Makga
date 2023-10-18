#pragma once

#include "config_world.h"
#include "../Common/server_base.h"
#include "../Common/acceptor_base.h"
#include "../Common/connector_base.h"
#include "../Common/manager_base.hpp"

struct ConfigWorld;
class JobHandler;
class NetHandler;

class ServerWorld : public ServerBase
{
public:
	ServerWorld() = delete;
	ServerWorld(ConfigWorld* _config);
	virtual ~ServerWorld();

	void Initialize() override;

	bool StartUp() override;
	bool StartEnd() override;

public:
	bool InitConnector();
	bool InitAcceptor();

private:
	JobHandler* m_job_handler;
	NetHandler* m_net_handler;

	std::vector<fb::eServerType> m_connect_list;
	ManagerBase<fb::eServerType, ConnectorBase*> m_connectors;

	std::vector<fb::eServerType> m_accept_list;
	ManagerBase<fb::eServerType, AcceptorBase*> m_acceptors;

	std::thread m_schedule;
};
