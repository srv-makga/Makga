module;

export module makga.lib.pattern.singleton;

import <memory>;
import <shared_mutex>;

export namespace makga::lib {
export template<typename T>
class Singleton
{
public:
	Singleton() = default;
	Singleton(const Singleton<T>&) = delete;
	Singleton(Singleton<T>&&) = delete;
	Singleton<T>& operator=(const Singleton<T>&) = delete;
	Singleton<T>& operator=(Singleton<T>&&) = delete;
	virtual ~Singleton() = default;

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