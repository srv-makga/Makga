#include "pch.h"
#include "acceptor_user.h"

AcceptorUser::AcceptorUser()
	: IocpServer(m_iocp_core, _ep, (SessionManager<std::shared_ptr<core::network::IocpSession>>*)&m_session_manager)
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