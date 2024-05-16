#pragma once

#include "../Core/ip_endpoint.h"
#include "../Core/iocp_service.h"
#include "../Core/rio_service.h"

class ServerProxy
{
public:
	using ServerService = core::network::ServerService;
	using IOCPService = core::network::IOCPService;
	using RIOService = core::network::RIOService;

public:
	ServerProxy() = delete;
	ServerProxy(const ServerProxy& other) = delete;
	ServerProxy(ServerProxy&& other) = delete;
	ServerProxy& operator=(const ServerProxy& other) = delete;
	ServerProxy& operator=(ServerProxy&& other) = delete;

	ServerProxy(core::network::IPEndPoint _endpoint, IOCPService* _service);
	ServerProxy(core::network::IPEndPoint _endpoint, RIOService* _service);
	virtual ~ServerProxy();

	bool Start();
	bool Stop();

private:
	std::shared_ptr<ServerService> m_service;
};