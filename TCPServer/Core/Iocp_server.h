#pragma once

#include "core_header.h"
#include "iocp_service.h"

namespace core {
namespace network {
class IocpServer : public IocpService
{
public:
	IocpServer() = delete;
	IocpServer(const IocpServer& other) = delete;
	IocpServer(IocpServer&& other) = delete;
	IocpServer& operator=(const IocpServer& other) = delete;
	IocpServer& operator=(IocpServer&& other) = delete;
	virtual ~IocpServer();

	virtual bool Start(function<void()> logicFunc) override;
	virtual bool Stop() override;

private:
	bool RunServer(function<void(void)> serverWork);

private:
	shared_ptr<IocpListener> m_iocpListener;

};
} //namespace network
} // namespace core
