#pragma once

#include "../Core/iocp_service.h"

class IocpServer : public core::network::IocpService
{
public:
	IocpServer();
	IocpServer(const IocpServer& _other) = delete;
	IocpServer(IocpServer&& _other) = delete;
	IocpServer& operator=(const IocpServer& _other) = delete;
	IocpServer& operator=(IocpServer&& _other) = delete;
	virtual ~IocpServer();

	bool Initialize() override;
	void Finalize() override;
	bool Start() override;
	bool Stop() override;

protected:
	bool Run(std::function<void()> _work);

private:
	// ¸®½¼Á¤º¸
};