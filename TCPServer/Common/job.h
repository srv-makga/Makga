#pragma once

#include "common_header.h"

class Job
{
public:
	Job(std::function<void()> callback) :m_callback(callback) {}
	
	template<typename T, typename Ret, typename...Args>
	Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		m_callback = [owner, memFunc, args...]()
			{
				(owner.get()->*memFunc)(args...);
			};
	}

	void Execute()
	{
		m_callback();
	}

private:
	std::function<void()> m_callback;
};