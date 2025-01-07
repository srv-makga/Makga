#include "pch.h"
#include "acceptor_user.h"

AcceptorUser::AcceptorUser()
	: IocpServer()
{
}

AcceptorUser::~AcceptorUser()
{
}

bool AcceptorUser::Initialize()
{
	return false;
}

void AcceptorUser::Finalize()
{
}

bool AcceptorUser::Setup(const core::network::IPEndPoint& _ep)
{
	return IocpServer::Setup(m_iocp_core, _ep, (SessionManager<std::shared_ptr<core::network::IocpSession>>*) & m_session_manager);
}
