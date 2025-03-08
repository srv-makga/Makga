#pragma once

#include "../core/singleton.hpp"
#include "../Common/proxy_server.h"

#include "acceptor_user.h"
#include "acceptor_admintool.h"

#include "connecter.h"

#include "session_user.h"
#include "session_world.h"
#include "session_community.h"
#include "session_dbagent.h"

class GameServer : public ProxyServer, public core::pattern::Singleton<GameServer>
{
public:
	GameServer(core::ServiceType _type);
	GameServer(const GameServer& _other) = delete;
	GameServer(GameServer&& _other) = delete;
	GameServer& operator=(const GameServer& _other) = delete;
	GameServer& operator=(GameServer&& _other) = delete;
	virtual ~GameServer();

	bool Initialize() override;
	void Finalize() override;

protected:
	bool StartUp() override;
	bool StartEnd() override;

public:
	AcceptorUser m_acceptor_user;
	AcceptorAdminTool m_acceptor_admintool;

	// @todo proxy 서버로 변경 필요
	std::shared_ptr<SessionWorld> m_session_world;
	std::shared_ptr<SessionCommunity> m_session_community;
	std::shared_ptr<SessionDBAgent> m_session_dbagent;
};

#define SERVER	GameServer::Instance()