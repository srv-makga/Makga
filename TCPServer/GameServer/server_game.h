#pragma once

#include "session_user.h"
#include "acceptor_user.h"


class GameServer
{
public:
	GameServer(const core::network::IPEndPoint& _ep, SessionManager<std::shared_ptr<SessionUser>>* _session_manager);
	GameServer() = delete;
	GameServer(const GameServer& _other) = delete;
	GameServer(GameServer&& _other) = delete;
	GameServer& operator=(const GameServer& _other) = delete;
	GameServer& operator=(GameServer&& _other) = delete;

	bool Initialize();
	void Finalize();

private:
	AcceptorUser m_acceptor_user;
};0