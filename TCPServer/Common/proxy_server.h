#pragma once

#include "service.h"

class ProxyServer : public Service
{
public:
	ProxyServer(eServerType _type);
	ProxyServer(const ProxyServer& other) = delete;
	ProxyServer(ProxyServer&& other) = delete;
	ProxyServer& operator=(const ProxyServer& other) = delete;
	ProxyServer& operator=(ProxyServer&& other) = delete;
	virtual ~ProxyServer();

public:
	bool Start(std::function<void(void)> _func) override;
	bool Stop() override;

private:
	std::shared_ptr<Service> m_service;
};