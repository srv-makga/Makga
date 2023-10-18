#pragma once

#include <mutex>
#include <memory>

namespace core {
namespace pattern {
template<typename T>
class Singleton
{
public:
	Singleton() = default;

	Singleton(const Singleton<T>& other) = delete;
	Singleton<T>& operator=(const Singleton<T>& rhs) = delete;

	Singleton(Singleton<T>&& other) = delete;
	Singleton<T>& operator=(Singleton<T>&& rhs) = delete;

	static T& Instance()
	{
		std::call_once(Singleton<T>::m_once_flag, []() {
			m_instance.reset(new T);
		});

		return *m_instance;
	}

	static T& Get()
	{
		std::call_once(Singleton<T>::m_once_flag, []() {
			m_instance.reset(new T);
			});

		return *m_instance;
	}

private:
	static std::unique_ptr<T> m_instance;
	static std::once_flag m_once_flag;
};

template<typename T>
std::unique_ptr<T> Singleton<T>::m_instance = nullptr;

template<typename T>
std::once_flag Singleton<T>::m_once_flag;
} // namespace pattern
} // namespace core