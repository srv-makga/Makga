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
// @brief 네트워크 서비스 클래스
class Service
{
public:
	Service() = default;
	virtual ~Service() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;
	virtual bool StartUp() = 0;
	virtual bool StartUpEnd() = 0;
	virtual bool Start()
	{
		if (StartUp() == false)
		{
			return false;
		}

		if (StartUpEnd() == false)
		{
			return false;
		}

		return true;
	}
	virtual bool Stop() = 0;
};
} // namespace server
} // namespace core