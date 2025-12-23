#include "pch.h"
#include "user_session.h"

UserSession::UserSession(std::shared_ptr<makga::network::IocpService> service)
	: makga::network::IocpSession(service)
{
}

UserSession::~UserSession()
{
}

void UserSession::ProcConnect()
{
}

void UserSession::ProcDisconnect()
{
}

std::size_t UserSession::ProcRecv(char* data, std::size_t bytes_transferred)
{
	return std::size_t();
}

void UserSession::ProcSend(std::int32_t bytes_transferred)
{
}
