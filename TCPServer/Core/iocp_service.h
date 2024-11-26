#pragma once

#include "net_header.h"
#include "socket_header.h"
#include "service.h"
#include "lock.h"

#ifdef _WIN32
namespace core {
namespace network {
class IocpCore;
class IocpSession;

// @detail 技记 包府
class IocpService : public server::Service
{
public:
	IocpService(ServiceType _service_type,
		std::shared_ptr<IocpCore> _iocp_core,
		std::function<std::shared_ptr<IocpSession>(void)> _alloc_session,
		std::function<void(std::shared_ptr<IocpSession>)> _dealloc_session);

	IocpService() = delete;
	IocpService(const IocpService& _other) = delete;
	IocpService(IocpService&& _other) = delete;
	IocpService& operator=(const IocpService& _other) = delete;
	IocpService& operator=(IocpService&& _other) = delete;
	virtual ~IocpService();

public: // Service
	bool Initialize() override;
	void Finalize() override;
	//bool Start() override;
	//bool Stop() override;

public:
	bool AddSession(std::shared_ptr<IocpSession> _session);
	bool DelSession(std::shared_ptr<IocpSession> _session);
	void DisconnectAllSession();
	void RunOnAllSessions(std::function<void(std::shared_ptr<IocpSession>)> _func);

	ServiceType GetServiceType() const;
	std::shared_ptr<IocpCore> GetIocpCore() const;
	std::size_t GetSessionCount() const;
	int GetThreadCount() const;

private:
	std::shared_ptr<IocpSession> AllocSession();
	void DeallocSession(std::shared_ptr<IocpSession> _session);

private:
	ServiceType m_service_type;
	std::shared_ptr<IocpCore> m_iocp_core;

	mutable core::RWMutex m_mutex_session;
	std::unordered_set<std::shared_ptr<IocpSession>> m_sessions;

	std::function<std::shared_ptr<IocpSession>(void)> m_alloc_session;
	std::function<void(std::shared_ptr<IocpSession>)> m_dealloc_session;

	unsigned int m_thread_count;
};
} // namespace network
} // namespace core
#endif // _WIN32