module;

#include <memory>
#include <functional>
#include <atomic>

export module makga.lib.pattern.objectpool;

import <vector>;
import makga.lib.lock;

export namespace makga::lib {

export template<typename T>
concept Resettable = requires(T& t) { t.Reset(); } || requires(T& t) { reset(t); };

export template<typename T> requires Resettable<T>
class ObjectPool
{
public:
	ObjectPool()
		: is_release_(std::make_shared<bool>(false))
	{
	}

	virtual ~ObjectPool()
	{
		Finalize();
	}

	struct CustomDeleter
	{
		std::weak_ptr<bool> wp;
		ObjectPool* pool;

		void operator()(T* object) const {
			auto sp = wp.lock();
			if (sp && false == *sp)
			{
				pool->ReleaseObject(object);
			}
			else
			{
				delete object;
			}
		}
	};

	std::unique_ptr<T, CustomDeleter> AcquireObject()
	{
		T* object = nullptr;

		makga::lib::LockGuard lock(mutex_);

		if (true == pool_.empty())
		{
			object = new T();
		}
		else
		{
			object = pool_.back();
			pool_.pop_back();
		}

		CustomDeleter deleter{ std::weak_ptr<bool>(is_release_), this };

		return std::unique_ptr<T, CustomDeleter>(object, deleter);
	}

	void ReleaseObject(T* object)
	{
		object->Reset();

		makga::lib::LockGuard lock(mutex_);
		pool_.emplace_back(object);
	}

	void CreateObject(std::size_t size)
	{
		makga::lib::LockGuard lock(mutex_);

		pool_.reserve(pool_.size() + size);

		for (std::size_t i = 0; i < size; ++i)
		{
			pool_.emplace_back(new T());
		}
	}

	void Finalize()
	{
		*is_release_ = true;

		makga::lib::LockGuard lock(mutex_);

		for (auto obj : pool_)
		{
			delete obj;
		}

		pool_.clear();
	}

protected:
	std::shared_ptr<bool> is_release_;
	std::vector<T*> pool_;
	makga::lib::Mutex mutex_;
};
} // namespace makga::lib