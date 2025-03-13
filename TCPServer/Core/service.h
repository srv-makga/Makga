#pragma once

#include <functional>

namespace core {
enum class ServiceType
{
	IOCP_SERVER,
	IOCP_CLIENT,
	RIO_SERVER,
	RIO_CLIENT
};

namespace server {
class Service
{
public:
	Service() = default;
	virtual ~Service() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;
	virtual bool Start(std::function<void(void)> _func) = 0;
	virtual bool Stop() = 0;
};
} // namespace server
} // namespace core