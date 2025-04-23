#pragma once

#include "net_header.h"
#include "socket_header.h"
#include "iocp_core.h"
#include "service.h"
#include "ip_endpoint.h"
#include "lock.h"
#include <unordered_set>

#ifdef _WIN32
namespace core {
namespace network {
class IocpSession;
class IocpService : public server::Service
{
public:
	IocpService(ServiceType _service_type);
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
	void Setup(std::shared_ptr<IocpCore> _iocp_core,
		std::function<std::shared_ptr<IocpSession>(void)> _alloc_session,
		std::function<void(std::shared_ptr<IocpSession>)> _dealloc_session);

	bool AddSession(std::shared_ptr<IocpSession> _session);
	bool DelSession(std::shared_ptr<IocpSession> _session);
	void DisconnectAllSession();
	void RunOnAllSessions(std::function<void(std::shared_ptr<IocpSession>)> _func);

	ServiceType GetServiceType() const;
	std::shared_ptr<IocpCore> GetIocpCore() const;
	std::size_t GetSessionCount() const;
	virtual std::size_t GetMaxSessionCount() const = 0;
	bool IsStart();
	int GetThreadCount() const;

	virtual const IPEndPoint& GetEndPoint() const = 0;
	virtual void SetEndPoint(const IPEndPoint& _ep) = 0;

public:
	std::shared_ptr<IocpSession> AllocSession();
	void DeallocSession(std::shared_ptr<IocpSession> _session);

protected:
	ServiceType m_service_type;
	std::shared_ptr<IocpCore> m_iocp_core = std::make_shared<IocpCore>();

	mutable core::RWMutex m_mutex_session;
	std::unordered_set<std::shared_ptr<IocpSession>> m_sessions;

	std::function<std::shared_ptr<IocpSession>(void)> m_alloc_session;
	std::function<void(std::shared_ptr<IocpSession>)> m_dealloc_session;

	std::atomic<bool> m_is_start;

	int m_thread_count;
};
} // namespace network
} // namespace core
#endif // _WIN32