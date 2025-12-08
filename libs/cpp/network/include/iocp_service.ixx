module;

#include <windows.h>

export module network.service.iocp;

import network.service;

export namespace makga::network {
class IocpService : public NetService
{
public:
	IocpService();
	virtual ~IocpService();

	bool Initialize() override;
	void Finalize() override;


};
} // namespace makga::network