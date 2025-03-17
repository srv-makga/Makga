#pragma once

#include "game_header.h"
#include "../Common/connector.h"

class AppConnector : public Connector
{
public:
	AppConnector() = default;
	virtual ~AppConnector();

	bool Initialize();
	void Finalize();

	bool AddSession(std::shared_ptr<Session_t> _session);

	bool SendToAll(std::shared_ptr<Packet> _packet);
	void DisconnectAll();

public:
	std::unordered_map<Session::Id, std::shared_ptr<Session_t>> m_sessions;
};