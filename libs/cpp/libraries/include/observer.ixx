module;

import <vector>;
import <memory>;
import <shared_mutex>;

export module makga.lib.pattern.observer;

template<typename T>
concept HasElementType = requires {
	typename T::element_type;
};

template<typename T>
concept HasNotify = requires(T observer) { { observer.Notify() } -> std::same_as<void>; };

template <typename T>
concept ObserverConcept = HasElementType<T> && std::is_same_v<T, std::shared_ptr<typename T::element_type>> && HasNotify<T>;

export namespace makga::lib {
template<ObserverConcept T>
class Observer
{
	using Mutex_t = std::shared_mutex;
	using ReadLock = std::shared_lock<Mutex_t>;
	using WriteLock = std::unique_lock<Mutex_t>;

public:
	Observer() = default;
	virtual ~Observer() = default;

	void Attach(T object)
	{
		WriteLock lock(mutex_);

		objects_.push_back(object);
	}

	void Detach(T object)
	{
		WriteLock lock(mutex_);

		auto iter = std::find(objects_.begin(), objects_.end(), object);
		if (objects_.end() == iter)
		{
			return;
		}

		// 호출 순서가 보장되지 않음
		std::swap(*iter, objects_.back());
		objects_.pop_back();
	}

	void OnUpdate() const
	{
		ReadLock lock(mutex_);

		for (const auto& object : objects_)
		{
			if (nullptr != object)
			{
				object->Notify();
			}
		}
	}

private:
	mutable std::shared_mutex mutex_;
	std::vector<T> objects_;
};
} // namespace makga::lib