#pragma once

#include "../Core/ip_endpoint.h"
#include "../Core/iocp_session.h"
#include "../Core/rio_session.h"
#include "session_manager.hpp"

class ServerProxy
{
public:
	using ServerService = core::server::Service;

public:
	ServerProxy() = delete;
	ServerProxy(const ServerProxy& other) = delete;
	ServerProxy(ServerProxy&& other) = delete;
	ServerProxy& operator=(const ServerProxy& other) = delete;
	ServerProxy& operator=(ServerProxy&& other) = delete;

	ServerProxy(core::network::IPEndPoint _endpoint, std::shared_ptr<SessionManager<core::network::IocpSession>> _session_manager);
	ServerProxy(core::network::IPEndPoint _endpoint, std::shared_ptr<SessionManager<core::network::RioSession>> _session_manager);
	virtual ~ServerProxy();

	bool Start();
	bool Stop();

private:
	std::shared_ptr<ServerService> m_service;
};