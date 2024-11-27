#pragma once

namespace core {
enum class ServiceType
{
	CLIENT,
	SERVER,
};
namespace server {
class Service
{
public:
	Service() = default;
	virtual ~Service() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;
	virtual bool Start() = 0;
	virtual bool Stop() = 0;
};
} // namespace server
} // namespace core