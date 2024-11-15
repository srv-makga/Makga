#include "session.h"
#include "../Core/lock.h"

template <typename T, typename ISession>
concept SessionType = std::is_pointer_v<T> && std::is_base_of_v<T, std::remove_pointer_t<ISession>>;

template<typename T>
//requires SessionType<T, ISession>
class SessionManager
{
public:
	using Container_t = std::unordered_map<SessionId_t, T>;
	using ContainerFree_t = std::queue<T>;
	using AllocFunctions_t = std::function<ISession*()>;
	using Mutex_t = core::RWMutex;

public:
	SessionManager() = default;
	virtual ~SessionManager() { Finalize(); }

	bool Initialize(AllocFunctions_t&& _func);
	bool Finalize();

	ISession* GetSession();
	ISession* FInd(SessionId_t _id) const;
	bool Erase(ISession* _session);

protected:
	void RegistedSession(ISession* _session);

	ISession* AllocSession() { return ::new T(); }
	void DeallocSession(ISession* _session) { ::delete _session; }

private:
	Mutex_t m_mutex;
	Container_t m_sessions;

	Mutex_t m_mutex_free;
	ContainerFree_t m_frees;
};

template<typename T>
inline bool SessionManager<T>::Initialize(AllocFunctions_t&& _func)
{
	m_alloc_func = std::move(_func);
	return true;
}

template<typename T>
inline bool SessionManager<T>::Finalize()
{
	{
		WriteLock lock(m_mutex);
		for (auto& iter : m_sessions)
		{
			DeallocSession(iter->second);
		}
	}

	{
		WriteLock lock(m_mutex_free);
		while (false == m_frees.empty())
		{
			ISession* session = m_frees.front();
			m_frees.pop();
			DeallocSession(session);
		}

	}

	return true;
}

template<typename T>
inline ISession* SessionManager<T>::GetSession()
{
	ISession* session = nullptr;

	do
	{
		WriteLock lock(m_mutex_free);
		if (false == m_frees.empty())
		{
			session = m_frees.front();
			m_frees.pop();
			break;
		}

		session = AllocSeesion();
	} while (false);
	
	RegistedSession(session);
	return session;
}

template<typename T>
inline ISession* SessionManager<T>::FInd(SessionId_t _id) const
{
	ReadLock lock(m_mutex);

	auto iter = m_sessions.find(_id);
	if (m_sessions.end() == iter)
	{
		return nullptr;
	}

	return iter->second;
}

template<typename T>
inline bool SessionManager<T>::Erase(ISession* _session)
{
	if (nullptr == _session)
	{
		return false;
	}

	bool ret = false;

	{
		WriteLock lock(m_mutex);
		ret = 0 < m_sessions.erase(_session->SessionId());
	}

	_session->FInalize();

	{
		WriteLock lock(m_mutex_free);
		m_frees.push(_session);
	}
}

template<typename T>
inline void SessionManager<T>::RegistedSession(ISession* _session)
{
	if (nullptr == _session)
	{
		return;
	}

	WriteLock lock(m_mutex);
	m_sessions.insert({ _session->SessionId(), _session });
}
