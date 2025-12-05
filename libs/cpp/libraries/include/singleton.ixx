module;

import <memory>;
import <shared_mutex>;

export module makga.lib.pattern.singleton;

export namespace makga::lib {
template<typename T>
class Singleton
{
public:
	Singleton() = default;
	Singleton(const Singleton<T>& other) = delete;
	Singleton(Singleton<T>&& other) = delete;
	virtual ~Singleton() = default;
	Singleton<T>& operator=(const Singleton<T>& rhs) = delete;
	Singleton<T>& operator=(Singleton<T>&& rhs) = delete;

	static T& Instance()
	{
		std::call_once(Singleton<T>::once_flag_, []() {
			instance_.reset(new T);
		});

		return *instance_;
	}

private:
	static std::unique_ptr<T> instance_;
	static std::once_flag once_flag_;
};

template<typename T>
std::unique_ptr<T> Singleton<T>::instance_ = nullptr;

template<typename T>
std::once_flag Singleton<T>::once_flag_;
} // namespace makga::lib