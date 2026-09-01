#include "pch.h"
#include "timer.h"

Timer::Timer()
	:is_running_(false)
	, last_update_tick_(0)
{
}

Timer::~Timer()
{
	Stop();
}

void Timer::Start()
{
	is_running_ = true;
	last_update_tick_ = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now().time_since_epoch()
	).count();
}

void Timer::Stop()
{
	is_running_ = false;
	jobs_.clear();
}

Timer& Timer::AddJob(std::function<void()> job, Tick interval, bool is_repeat)
{
	if (nullptr == job || 0 >= interval)
	{
		return *this;
	}

	Tick current_tick = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now().time_since_epoch()
	).count();

	TimerJob timer_job;
	timer_job.job = job;
	timer_job.interval = interval;
	timer_job.next_execute_tick = current_tick + interval;
	timer_job.is_repeat = is_repeat;
	jobs_.emplace(timer_job.next_execute_tick, timer_job);
	return *this;
}

void Timer::OnUpdate()
{
	if (false == is_running_)
	{
		return;
	}

	Tick current_tick = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now().time_since_epoch()
	).count();
	last_update_tick_ = current_tick;

	auto it = jobs_.begin();
	while (it != jobs_.end())
	{
		if (it->first > current_tick)
		{
			break;
		}

		TimerJob timer_job = it->second;
		it = jobs_.erase(it);

		timer_job.job();

		if (true == timer_job.is_repeat)
		{
			timer_job.next_execute_tick = current_tick + timer_job.interval;
			jobs_.emplace(timer_job.next_execute_tick, timer_job);
		}
	}
}
