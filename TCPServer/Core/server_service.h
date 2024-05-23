#pragma once

namespace core {
namespace server {
class ServerService
{
public:
	ServerService() = default;
	virtual ~ServerService() = default;

	virtual bool Start() = 0;
	virtual bool Stop() = 0;
};
} // namespace server
} // namespace core