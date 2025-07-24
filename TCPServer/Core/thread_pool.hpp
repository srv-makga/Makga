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
template<typename T>
class ThreadPool
{
public:
	using TaskType = T;
	using Threads_t = std::vector<std::thread>;

public:
	ThreadPool()
		: m_stop(true)
	{
	}

	~ThreadPool()
	{
		Stop();
	}

	bool Initialize()
	{
		return true;
	}

	void Finalize()
	{
		Stop();
		m_workers.clear();
	}

	void InitThread(std::size_t _thread_count)
	{
		if (false == m_stop)
		{
			Stop();
		}

		if (false == m_workers.empty())
		{
			Threads_t temp;
			temp.swap(m_workers);
		}

		m_workers.reserve(_thread_count);

		for (std::size_t i = 0; i < _thread_count; ++i)
		{
			m_workers.emplace_back([this] {
				TaskType task = nullptr;
				while (true)
				{
					{
						std::unique_lock<std::mutex> lock(m_mutex);
						this->m_cv.wait(lock,
							[this] { return this->m_stop || false == this->m_tasks.empty(); });

						if (this->m_stop && true == this->m_tasks.empty())
						{
							return;
						}

						task = std::move(this->m_tasks.front());
						m_tasks.pop();
					}

					if (nullptr != task)
					{
						task->Execute();
					}
				}
			});
		}
	}

	void Push(TaskType&& _task)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_tasks.push(std::move(_task));
		m_cv.notify_one();
	}

	ThreadPool& operator<<(TaskType&& _task)
	{
		Push(std::forward<TaskType>(_task));
		return *this;
	}

	void Start()
	{
		m_stop = false;
	}

	void Stop()
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_stop = true;

		m_cv.notify_all();

		for (std::thread& worker : m_workers)
		{
			worker.join();
		}
	}

	bool IsStop() const
	{
		return m_stop;
	}

	bool IsEmpty() const
	{
		std::unique_lock lock(m_mutex);
		return true == m_tasks.empty();
	}

protected:
	void Run()
	{
		TaskType task = nullptr;

		while (true)
		{
			{
				std::unique_lock<std::mutex> lock(m_mutex);

				this->m_cv.wait(lock,
					[this] { return this->m_stop || false == this->m_tasks.empty();}
				);

				if (this->m_stop && this->m_tasks.empty())
				{
					return;
				}

				task = std::move(this->m_tasks.front());
				m_tasks.pop();
			}

			if (nullptr != task)
			{
				task();
			}
		}
	}

private:
	Threads_t m_workers;
	std::queue<TaskType> m_tasks;

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