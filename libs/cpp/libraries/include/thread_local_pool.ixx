module;

export module makga.lib.thread_local_pool;

namespace makga::lib {
export template<typename T>
class ThreadLocalPool
{
public:
	static T* Allocate()
	{
		if (nullptr != free_list_)
		{
			T* obj = free_list_;
			free_list_ = free_list_->next;
			return obj;
		}

		return new T();
	}

	static void Deallocate(T* obj)
	{
		obj->next = free_list_;
		free_list_ = obj;
	}

private:
	static thread_local T* free_list_;
};

template<typename T>
thread_local T* ThreadLocalPool<T>::free_list_ = nullptr;
} // namespace makga::lib