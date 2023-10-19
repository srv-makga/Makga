#include "pch.h"
#include "acceptor_game.h"
#include "session_game.h"
#include "config_world.h"

AcceptorGame::AcceptorGame()
{
}

AcceptorGame::~AcceptorGame()
{
}

void AcceptorGame::Initialize()
{
	AcceptorBase::Initialize();
}

void AcceptorGame::InitSession(std::size_t _max_session)
{
	for (std::size_t i = 0; i < _max_session; ++i)
	{
		SessionBase* new_session = ::new SessionGame(CONFIG.buffer_size_read);
		new_session->Initialize();
		m_free_sessions.push(new_session);
	}
}

SessionBase* AcceptorGame::AllocSession()
{
	SessionBase* new_session = nullptr;

	std::unique_lock lock(m_free_mutex);
	if (true == m_free_sessions.empty())
	{
		new_session = ::new SessionGame(CONFIG.buffer_size_read);
	}
	else
	{
		new_session = m_free_sessions.front();
		m_free_sessions.pop();
	}

	new_session->Initialize();
	new_session->SetNetActor(this);

	AddWaitSession(new_session);
	
	return new_session;
}

void AcceptorGame::ReallocSession(SessionBase* _session)
{
	RemoveSession(_session);

	_session->Initialize();

	std::unique_lock lock(m_wait_mutex);
	m_wait_sessions.insert(_session);
}