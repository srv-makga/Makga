#include "pch.h"
#include "user_server.h"
#include "user_session.h"

UserServer::UserServer(makga::network::IPEndPoint ep, std::shared_ptr<makga::network::IocpCore> core)
	: IocpServer(ep, core)
{
}

UserServer::~UserServer()
{
}

void UserServer::CreateSession(std::size_t max_connect_count)
{
	for (std::size_t i = 0; i < max_connect_count; ++i)
	{
		auto session = std::make_shared<UserSession>(shared_from_this());
		if (nullptr == session)
		{
			continue;
		}

		session->SetSessionId(++next_session_id_);

		free_sessions_.push(session);
	}
}

bool UserServer::StartUp()
{
    return false;
}

bool UserServer::StartUpEnd()
{
	return false;
}

bool UserServer::Stop()
{
	return false;
}

