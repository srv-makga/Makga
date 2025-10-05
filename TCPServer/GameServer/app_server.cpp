#include "pch.h"
#include "app_server.h"

AppServer::AppServer(std::shared_ptr<core::network::IocpCore> _core, const core::network::IPEndPoint& _endpoint)
	: Server_t(_core, _endpoint)
{
}

AppServer::~AppServer()
{
	Finalize();
}

bool AppServer::Initialize()
{
	if (false == Server_t::Initialize())
	{
		LOG_ERROR << "IocpServer initialize failed.";
		return false;
	}
	return true;
}

void AppServer::Finalize()
{
	Server_t::Finalize();
}