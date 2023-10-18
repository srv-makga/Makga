#pragma once

#include "core_header.h"
#include <thread>

namespace core {
namespace thread {
class Thread
{
public:
	using Uid_t = uint32_t;

	enum class State_e
	{
		Waiting = 0,
		Running,
		Suspend
	};

	enum class Type_e
	{
		Network = 1,
		Work,
		DB
	};

public:
	Thread();
	Thread(const Thread&) = delete;
	Thread(Thread&&) = delete;
	virtual ~Thread();

	Thread& operator=(const Thread&) = delete;
	Thread& operator=(Thread&&) = delete;

	void Start();
	void Stop();

	/*
	* @brief 스레드 일시 정지
	*/
	void Suspend();

	/*
	* @brief 스레드 재개
	*/
	void Resume();

	Uid_t Uid() const;
	State_e State() const;
	bool IsStop() const;

protected:
	virtual void Run() = 0;
	std::atomic<bool> m_is_stop;

private:
	Uid_t m_uid;
	State_e m_state;
	std::thread m_thread;

	static std::atomic<Uid_t> s_uid;
};
} // namespace thread
} // namespace core