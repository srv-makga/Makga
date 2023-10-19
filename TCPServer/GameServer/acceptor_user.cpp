#include "pch.h"
#include "acceptor_user.h"
#include "session_user.h"
#include "config_game.h"

AcceptorUser::AcceptorUser()
{
}

AcceptorUser::~AcceptorUser()
{
}

void AcceptorUser::Initialize()
{
	AcceptorBase::Initialize();
}

void AcceptorUser::InitSession(std::size_t _max_session)
{
	for (std::size_t i = 0; i < _max_session; ++i)
	{
		SessionBase* new_session = new SessionUser;
		new_session->Initialize();
		m_free_sessions.push(new_session);
	}
}

SessionBase* AcceptorUser::AllocSession()
{
	SessionBase* new_session = nullptr;

	std::unique_lock lock(m_free_mutex);
	if (true == m_free_sessions.empty())
	{
		new_session = new SessionUser;
	}
	else
	{
		new_session = m_free_sessions.front();
		m_free_sessions.pop();
	}

	new_session->Initialize();
	new_session->SetNetActor(this);

	AddSession(new_session);
	
	return new_session;
}

void AcceptorUser::ReallocSession(SessionBase* _session)
{
	RemoveSession(_session);

	_session->Finallize();

	std::unique_lock lock(m_free_mutex);
	m_free_sessions.push(_session);
}
