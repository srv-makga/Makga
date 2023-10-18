#pragma once

#include <vector>
#include <thread>
#include <condition_variable>
#include <functional>

#include "queue_swap.hpp"

namespace core {
namespace thread {
/*
* @brief 단일 작업을 반복하는 스레드 그룹
*/
class ThreadGroup
{
public:
	using Task_t = std::function<void(void)>;

public:
	ThreadGroup();
	ThreadGroup(const ThreadGroup&) = delete;
	ThreadGroup(ThreadGroup&&) = delete;
	~ThreadGroup();

	ThreadGroup& operator=(const ThreadGroup&) = delete;
	ThreadGroup& operator=(ThreadGroup&&) = delete;

	void Initialize();
	void InitThread(std::size_t _thread_count, Task_t&& _task);
	void Stop();
	void Resume();

private:
	std::vector<std::thread> m_workers;
	Task_t m_task;

	std::atomic<bool> m_stop;
};
} // namespace thread
} // namespace core