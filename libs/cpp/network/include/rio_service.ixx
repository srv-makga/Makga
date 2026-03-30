module;

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif

export module makga.network.rio.service;

export import makga.network.service;
export import makga.network.rio.core;
export import makga.network.jobhandler;
export import makga.network.nethandler;
export import makga.network.endpoint;
export import makga.network.session;

namespace makga::network {
export class RioService : public NetService
{
public:
	RioService(NetServiceType service_type, std::shared_ptr<RioCore> core);
	virtual ~RioService();

	bool Initialize();
	void Finalize();

	std::shared_ptr<RioCore> GetRioCore() const;
	std::shared_ptr<NetHandler> GetNetHandler() const;
	void SetNetHandler(std::shared_ptr<NetHandler> handler);
	std::shared_ptr<JobHandler> GetJobHandler() const;
	void SetJobHandler(std::shared_ptr<JobHandler> handler);
	bool IsRunning() const;

	RIO_CQ GetRioCompletionQueue() const;
	RIO_RQ GetRioRequestQueue() const;
	void SetRioRequestQueue(RIO_RQ rq);
	const RIO_EXTENSION_FUNCTION_TABLE& GetRioFunctionTable() const;

	virtual IPEndPoint GetEndPoint() const = 0;
	virtual std::size_t GetConnectCount() const = 0;
	virtual std::size_t GetMaxConnectCount() const = 0;

	virtual std::shared_ptr<NetSession> AllocSession() = 0;
	virtual void DeallocSession(std::shared_ptr<NetSession> session) = 0;

	// Worker thread control
	bool StartWorkerThreads(int thread_count = 1);
	void StopWorkerThreads();

protected:
	void WorkerThread();
	void ProcessCompletions();

protected:
	std::shared_ptr<RioCore> core_;
	std::shared_ptr<NetHandler> net_handler_;
	std::shared_ptr<JobHandler> job_handler_;
	bool is_running_;

	RIO_CQ rio_cq_;
	RIO_RQ rio_rq_;
	RIO_EXTENSION_FUNCTION_TABLE rio_func_table_;

	std::atomic<bool> worker_running_;
	std::vector<std::thread> worker_threads_;
};
} // namespace makga::network