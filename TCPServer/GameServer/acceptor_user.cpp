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
	return IocpServer::Setup(m_iocp_core,
		_ep,
		[]() { return std::make_shared<SessionUser>(); },
		[](std::shared_ptr<core::network::Session> _session) { if (nullptr != _session) _session.reset(); }
	);
}
