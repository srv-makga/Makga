#pragma once

#include "config_global.h"
#include "../Common/server_base.h"
#include "../Common/acceptor_base.h"
#include "../Common/connector_base.h"
#include "../Common/manager_base.hpp"

struct ConfigGlobal;

class ServerGlobal : public ServerBase
{
public:
	ServerGlobal() = delete;
	ServerGlobal(ConfigGlobal* _cconfig);
	virtual ~ServerGlobal();

	void Initialize() override;

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

	std::vector<fb::eServerType> m_accept_list;
	ManagerBase<fb::eServerType, AcceptorBase*> m_acceptors;

	std::thread m_scheduler;
};
