#pragma once

#include "net_actor.h"

struct ConnectorConfig
{

};

// @brief 특정 서버에 접속한 세션 관리용 클래스
class ComConnector
{
	using Session_t = core::network::Session;

public:
	ComConnector(std::shared_ptr<ConnectorConfig> _config);
	virtual ~ComConnector();

public:
	bool Initialize();
	void Finalize();

public:
	bool OnConnected(std::shared_ptr<Session_t> _session, IOContext_t* _context);
	bool OnReceived(std::shared_ptr<Session_t> _session, DWORD _bytes_transferred, IOContext_t* _context);
	bool OnSent(std::shared_ptr<Session_t> _session, DWORD _bytes_transferred, IOContext_t* _context);
	bool OnClosed(std::shared_ptr<Session_t> _session, IOContext_t* _context);

private:
	std::shared_ptr<ConnectorConfig> m_config;
};