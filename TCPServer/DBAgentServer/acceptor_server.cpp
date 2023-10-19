#include "pch.h"
#include "acceptor_server.h"
#include "session_server.h"
#include "config_dbagent.h"

AcceptorServer::AcceptorServer()
{
}

AcceptorServer::~AcceptorServer()
{
}

void AcceptorServer::Initialize()
{
	AcceptorBase::Initialize();
}

void AcceptorServer::InitSession(std::size_t _max_session)
{
	for (std::size_t i = 0; i < _max_session; ++i)
	{
		SessionBase* new_session = new SessionServer(CONFIG.session_user_buffer_size);
		new_session->Initialize();
		m_free_sessions.push(new_session);
	}
}

SessionBase* AcceptorServer::AllocSession()
{
	SessionBase* new_session = nullptr;

	std::unique_lock lock(m_free_mutex);
	if (true == m_free_sessions.empty())
	{
		new_session = new SessionServer(CONFIG.session_user_buffer_size);
	}
	else
	{
		new_session = m_free_sessions.front();
		m_free_sessions.pop();
	}

	new_session->Initialize();
	new_session->SetNetActor(this);

	{
		std::unique_lock lock(m_wait_mutex);
		m_wait_sessions.insert(new_session);
	}

	return new_session;
}

void AcceptorServer::ReallocSession(SessionBase* _session)
{
	RemoveSession(_session);

	_session->Finallize();

	std::unique_lock lock(m_free_mutex);
	m_free_sessions.push(_session);
}
