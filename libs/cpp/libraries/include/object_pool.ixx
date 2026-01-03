module;

#include <memory>
#include <functional>
#include <atomic>

export module makga.lib.pattern.objectpool;

import <vector>;

export namespace makga::lib {
export template<typename T>
class ObjectPool
{
public:
	ObjectPool()
		: is_release_(false)
	{
	}

	virtual ~ObjectPool()
	{
		Finalize();
	}

	std::unique_ptr<T, std::function<void(T*)>> AcquireObject()
	{
		T* object = nullptr;

		if (true == pool_.empty())
		{
			object = new T();
		}
		else
		{
			object = pool_.back();
			pool_.pop_back();
		}

		auto wp = std::weak_ptr<bool>(is_release_);
		auto deleter = [wp, this](T* obj)
		{
			auto sp = wp.lock();
			if (sp && false == sp->load())
			{
				pool_.emplace_back(obj);
			}
			else
			{
				delete obj;
			}
		};

		return std::unique_ptr<T, decltype(deleter)>(object, deleter);
	}

	void CreateObject(std::size_t size)
	{
		pool_.reserve(size);

		for (std::size_t i = 0; i < size; ++i)
		{
			pool_.emplace_back(new T());
		}
	}

	void Finalize()
	{
		is_release_.store(true);

		for (auto obj : pool_)
		{
			delete obj;
		}

		pool_.clear();
	}

protected:
	std::shared_ptr<bool> is_release_;
	std::vector<T*> pool_;
};
} // namespace makga::lib