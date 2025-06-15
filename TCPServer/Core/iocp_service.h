#pragma once

#include "net_header.h"
#include "socket_header.h"
#include "iocp_core.h"
#include "service.h"
#include "ip_endpoint.h"
#include "lock.h"
#include <unordered_set>

class NetHandler;
class JobHandler;

#ifdef _WIN32
namespace core {
namespace network {
class IocpSession;
class IocpService : public server::Service
{
public:
	IocpService(ServiceType _service_type, std::shared_ptr<IocpCore> _core);
	IocpService(const IocpService& _other) = delete;
	IocpService(IocpService&& _other) = delete;
	IocpService& operator=(const IocpService& _other) = delete;
	IocpService& operator=(IocpService&& _other) = delete;
	virtual ~IocpService();

public: // Service
	bool Initialize() override;
	void Finalize() override;
	void InitHandler(std::shared_ptr<NetHandler> _net_handler, std::shared_ptr<JobHandler> _job_handler);

	ServiceType GetServiceType() const;
	std::shared_ptr<IocpCore> GetIocpCore() const;
	bool IsRunning() const;

	virtual const std::shared_ptr<core::network::IPEndPoint> GetEndPoint() const = 0;
	virtual std::size_t GetConnectCount() const = 0;
	virtual std::size_t GetMaxConnectCount() const = 0;

	// @brief free 관련 함수
	virtual std::shared_ptr<IocpSession> AllocSession() = 0;
	virtual void DeallocSession(std::shared_ptr<IocpSession> _session) = 0;

	// @brief using 관련 함수
	virtual bool AddSession(std::shared_ptr<IocpSession> _session) = 0;
	virtual void RemoveSession(std::shared_ptr<IocpSession> _session) = 0;

protected:
	virtual bool Run(std::function<void(void)> _work) = 0;

protected:
	ServiceType m_service_type;
	std::shared_ptr<IocpCore> m_iocp_core;
	std::shared_ptr<NetHandler> m_net_handler;
	std::shared_ptr<JobHandler> m_job_handler;
	bool m_is_running;
};
} // namespace network
} // namespace core
#endif // _WIN32