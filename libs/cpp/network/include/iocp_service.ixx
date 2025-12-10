module;

#include <windows.h>
#include <memory>

export module makga.network.iocp.service;

import makga.network.nethandler;
import makga.network.jobhandler;
import makga.network.service;
import makga.network.endpoint;
import makga.network.iocp.core;

export namespace makga::network {
class IocpSession;
export class IocpService : public NetService
{
public:
	IocpService(std::shared_ptr<IocpCore> core);
	IocpService(const IocpService&) = delete;
	IocpService(IocpService&&) = delete;
	IocpService& operator=(const IocpService&) = delete;
	IocpService& operator=(IocpService&&) = delete;
	virtual ~IocpService();

	bool Initialize();
	void Finalize();

	std::shared_ptr<NetHandler> GetNetHandler() const;
	void SetNetHandler(std::shared_ptr<NetHandler> handler);
	std::shared_ptr<JobHandler> GetNetHandler() const;
	void SetJobHandler(std::shared_ptr<JobHandler> handler);

	std::shared_ptr<IocpCore> GetIocpCore() const;
	bool IsRunning() const;

public:
	virtual IPEndPoint GetEndPoint() const = 0;
	virtual std::size_t GetConnectCount() const = 0;
	virtual std::size_t GetMaxConnectCount() const = 0;

	virtual std::shared_ptr<IocpSession> AllocSession() = 0;
	virtual void DeallocSession(std::shared_ptr<IocpSession> session) = 0;

public: // NetService
	bool Stop() override;

protected:
	bool StartUp() override;
	bool StartUpEnd() override;

protected:
	std::shared_ptr<IocpCore> core_;
	std::shared_ptr<NetHandler> net_handler_;
	std::shared_ptr<JobHandler> job_handler_;
	bool is_running_;
};
} // namespace makga::network