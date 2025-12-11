module;

#include <windows.h>
#include <memory>

export module makga.network.iocp.service;

export import makga.network.nethandler;
export import makga.network.jobhandler;
export import makga.network.service;
export import makga.network.endpoint;
export import makga.network.iocp.core;

export namespace makga::network {
class IocpSession;
export class IocpService : public NetService
{
public:
	IocpService(NetServiceType service_type, std::shared_ptr<IocpCore> core);
	IocpService(const IocpService&) = delete;
	IocpService(IocpService&&) = delete;
	IocpService& operator=(const IocpService&) = delete;
	IocpService& operator=(IocpService&&) = delete;
	virtual ~IocpService();

	bool Initialize();
	void Finalize();

	std::shared_ptr<IocpCore> GetIocpCore() const;
	std::shared_ptr<NetHandler> GetNetHandler() const;
	void SetNetHandler(std::shared_ptr<NetHandler> handler);
	std::shared_ptr<JobHandler> GetJobHandler() const;
	void SetJobHandler(std::shared_ptr<JobHandler> handler);
	bool IsRunning() const;

public:
	virtual IPEndPoint GetEndPoint() const = 0;
	virtual std::size_t GetConnectCount() const = 0;
	virtual std::size_t GetMaxConnectCount() const = 0;

	virtual std::shared_ptr<IocpSession> AllocSession() = 0;
	virtual void DeallocSession(std::shared_ptr<IocpSession> session) = 0;

protected:
	std::shared_ptr<IocpCore> core_;
	std::shared_ptr<NetHandler> net_handler_;
	std::shared_ptr<JobHandler> job_handler_;
	bool is_running_;
};
} // namespace makga::network