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
		auto session = std::make_shared<UserSession>(shared_from_this(), ++next_session_id_);
		if (nullptr == session)
		{
			continue;
		}

		free_sessions_.push(session);
	}
}

