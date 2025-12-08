module;

#include <windows.h>

export module makga.network.iocp.service;

import makga.network.service;

export namespace makga::network {
export class IocpService : public NetService
{
public:
	IocpService();
	virtual ~IocpService();

	bool Initialize() override;
	void Finalize() override;


};
} // namespace makga::network