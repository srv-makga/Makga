#pragma once

struct TimerJob
{
	std::function<void()> job;
	Tick interval;
	Tick next_execute_tick;
	bool is_repeat;
};

class Timer
{
	using Tick = std::chrono::milliseconds::rep;

public:
	Timer();
	virtual ~Timer();
	void Start();
	void Stop();

	Timer& AddJob(std::function<void()> job, Tick interval, bool is_repeat);

	void OnUpdate();

private:
	std::multimap<Tick, TimerJob> jobs_;
	bool is_running_;
	Tick last_update_tick_;
};