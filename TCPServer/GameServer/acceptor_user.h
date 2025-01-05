#pragma once

#include "game_header.h"
#include "session_user.h"
#include "session_manager_user.h"
#include "../Common/iocp_server.h"

class AcceptorUser : public IocpServer
{
public:
	AcceptorUser();
	virtual ~AcceptorUser();

	AcceptorUser() = delete;
	AcceptorUser(const AcceptorUser& _other) = delete;
	AcceptorUser(AcceptorUser&& _other) = delete;
	AcceptorUser& operator=(const AcceptorUser& _other) = delete;
	AcceptorUser& operator=(AcceptorUser&& _other) = delete;

	bool Initialize();
	void Finalize();

private:
	core::network::IocpCore m_core;
	SessionManagerUser m_session_manager;
};