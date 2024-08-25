#pragma once

#include <vector>
#include <thread>
#include <condition_variable>
#include <functional>

#include "queue_swap.hpp"

namespace core {
namespace thread {
/*
* @brief ���� �۾��� �ݺ��ϴ� ������ �׷�
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
	void CreateThread(Task_t&& _task, std::size_t _thread_count = 1);
	void Stop();
	void Resume();

private:
	std::vector<std::thread> m_workers;
	Task_t m_task;

	std::atomic<bool> m_stop;
};
} // namespace thread
} // namespace core