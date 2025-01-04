#pragma once

#include "session_user.h"
#include "../Core/singleton.hpp"
#include "../Common/session_manager.hpp"

class SessionManagerUser : public SessionManager<std::shared_ptr<SessionUser>>, public core::pattern::Singleton<SessionManagerUser>
{
public:
	SessionManagerUser() = default;
	~SessionManagerUser() = default;

private:
	std::shared_ptr<SessionUser> Alloc() override
	{
		return std::make_shared<SessionUser>(++m_id_sequence);
	}

	void Dealloc(std::shared_ptr<SessionUser> _session) override
	{
		_session.reset();
	}

private:
	std::atomic<SessionUser::Id> m_id_sequence = 0;
};

#define SESSION_MANAGER_USER SessionManagerUser::Instance()