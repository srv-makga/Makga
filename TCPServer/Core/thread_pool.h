#pragma once

#include "lock.h"
#include "queue_swap.hpp"

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace core {
namespace thread {
class ThreadPool
{
public:
	using Task_t = std::function<void(void)>;

public:
	ThreadPool();
	~ThreadPool();

	void Initialize();
	void InitThread(std::size_t _thread_count);

	void AddTask(Task_t&& _task);

	ThreadPool& operator<<(Task_t&& _task);

	void Stop();

protected:
	void Run();

private:
	std::vector<std::thread> m_workers;
	std::queue<Task_t> m_tasks;

	std::atomic<bool> m_stop;

	mutable std::mutex m_mutex;
	std::condition_variable m_cv;
};

//template<typename F, typename... Args>
//auto thread_pool::enqueue(F&& f, Args&&... args, task_t&& _callback) -> std::future<std::invoke_result_t<F, Args...>> {
//	using return_type = std::invoke_result_t<F, Args...>;
//
//	if (stop)
//	{
//		return std::future<std::invoke_result_t<F, Args...>>();
//	}
//
//	auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
//	std::future<return_type> future = task->get_future();
//
//	{
//		std::unique_lock<std::mutex> lock(m_mutex);
//		m_tasks.emplace([task, callback = std::move(_callback)]() { (*task)(); callback() });
//	}
//
//	m_cv.notify_one();
//	return future;
//}
} // namespace thread
} // namespace core