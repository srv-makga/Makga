#pragma once

#include "core_header.h"
#include "iocp_service.h"

namespace core {
namespace network {
class IocpCore;
class IocpListener;

class IocpServer : public IocpService
{
public:
	IocpServer(std::shared_ptr<IocpCore> _core, );
	IocpServer() = delete;
	IocpServer(const IocpServer& _other) = delete;
	IocpServer(IocpServer&& _other) = delete;
	IocpServer& operator=(const IocpServer& _other) = delete;
	IocpServer& operator=(IocpServer&& _other) = delete;
	virtual ~IocpServer();

	virtual bool Start(std::function<void()> logicFunc) override;
	virtual bool Stop() override;

private:
	bool RunServer(std::function<void(void)> serverWork);

private:
	std::shared_ptr<IocpListener> m_istener;

};
} //namespace network
} // namespace core
