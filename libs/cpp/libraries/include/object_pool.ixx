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

	struct CustomDeleter
	{
		std::weak_ptr<bool> wp;
		ObjectPool* pool;

		void operator()(T* object) const {
			auto sp = wp.lock();
			if (sp && false == sp->load())
			{
				pool->pool_.emplace_back(object);
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
		*is_release_ = true;

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