module;

#include <memory>
#include <functional>
#include <thread>

export module makga.network.jobhandler;

import makga.network.session;

export namespace makga::network {
export class Job
{
public:
	Job()
		: session_(nullptr)
		, func_(nullptr)
		, thread_id_() {}

	void Initialize()
	{
		session_ = nullptr;
		func_ = nullptr;
		thread_id_ = std::thread::id();
	}

	void Set(std::shared_ptr<NetSession> session, std::function<void(void)>& func, std::thread::id id)
	{
		session_ = session;
		func_ = func;
		thread_id_ = id;
	}

	void Execute()
	{
		if (func_)
		{
			func_();
		}
	}

	std::thread::id GetThreadId() const
	{
		return thread_id_;
	}

private:
	std::shared_ptr<NetSession> session_;
	std::function<void(void)> func_;
	std::thread::id thread_id_;
};

export class JobHandler
{
public:
	JobHandler() = default;
	virtual ~JobHandler() = default;

	virtual bool CreateThread(std::size_t thread_count = 1) = 0;
	virtual bool Start() = 0;
	virtual void Stop() = 0;

	virtual void Push(std::shared_ptr<Job> job) = 0;

protected:
	virtual std::shared_ptr<Job> Pop() = 0;
};
} // namespace makga::network