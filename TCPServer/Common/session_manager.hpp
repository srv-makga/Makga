#include "session.h"
#include "../Core/lock.h"

//template <typename T, typename SessionType>
//concept SessionType = std::is_pointer_v<T> && std::is_base_of_v<T, std::remove_pointer_t<SessionType>>;
//requires SessionType<T, ISession>

template<typename T>
class SessionManager
{
public:
	using Container_t = std::unordered_map<SessionId_t, T>;
	using ContainerFree_t = std::queue<T>;
	using Mutex_t = core::RWMutex;

public:
	SessionManager() = default;
	virtual ~SessionManager() { Finalize(); }

	bool Initialize(std::size_t _free_count)
	{
		for (std::size_t i = 0; i < _free_count; ++i)
		{
			m_sessions_free.push(Alloc());
		}

		return true;
	}

	void Finalize()
	{
		{
			core::WriteLock lock(m_mutex);
			for (auto& iter : m_sessions)
			{
				iter.second->Finalize();
				Dealloc(iter.second);
			}

			m_sessions.clear();
		}

		{
			core::WriteLock lock(m_mutex_free);
			while (false == m_sessions_free.empty())
			{
				T session = m_sessions_free.front();
				m_sessions_free.pop();
				Dealloc(session);
			}
		}
	}

	T GetSession()
	{
		T session = nullptr;

		do
		{
			core::WriteLock lock(m_mutex_free);
			if (false == m_sessions_free.empty())
			{
				session = m_sessions_free.front();
				m_sessions_free.pop();
				break;
			}

		} while (false);

		if (nullptr == session)
		{
			return nullptr;
		}
		
		RegistedSession(session);
		return session;
	}

	T FInd(SessionId_t _id) const
	{
		core::ReadLock lock(m_mutex);

		auto iter = m_sessions.find(_id);
		if (m_sessions.end() == iter)
		{
			return nullptr;
		}

		return iter->second;
	}

	bool Erase(T _session)
	{
		if (nullptr == _session)
		{
			return false;
		}

		bool ret = false;

		{
			core::WriteLock lock(m_mutex);
			ret = 0 < m_sessions.erase(_session->GetSessionId());
		}

		_session->Finalize();

		{
			core::WriteLock lock(m_mutex_free);
			m_sessions_free.push(_session);
		}

		return ret;
	}

protected:
	void RegistedSession(T _session)
	{
		if (nullptr == _session)
		{
			return;
		}

		core::WriteLock lock(m_mutex);
		m_sessions.insert({ _session->GetSessionId(), _session });
	}

	virtual T Alloc() = 0;
	virtual void Dealloc(T _session) = 0;

private:
	Mutex_t m_mutex;
	Container_t m_sessions;

	Mutex_t m_mutex_free;
	ContainerFree_t m_sessions_free;
};