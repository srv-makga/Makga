#pragma once  

#include <queue>  
#include <mutex>  
#include <condition_variable>  
#include <memory>  

template <typename T>  
struct is_disallowed_ptr : std::false_type {};  

template <typename U, typename D>  
struct is_disallowed_ptr<std::unique_ptr<U, D>> : std::true_type {};  

template <typename U>  
struct is_disallowed_ptr<std::shared_ptr<U>> : std::true_type {};  

template <typename U>  
struct is_disallowed_ptr<U*> : std::true_type {};  

template <typename T>  
constexpr bool is_disallowed_ptr_v = is_disallowed_ptr<T>::value;  

namespace core {  
namespace queue {  
template <typename T, typename Compare = std::less<T>>  
class ThreadSafePriorityQueue  
{  
	using QueueType = std::priority_queue<T, std::vector<T>, Compare>;

public:
	ThreadSafePriorityQueue()
		: m_push_pointer(&m_queue1), m_pop_pointer(&m_queue2)
	{
	}

	~ThreadSafePriorityQueue()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		while (!m_push_pointer->empty())
		{
			m_push_pointer->pop();
		}
		while (!m_pop_pointer->empty())
		{
			m_pop_pointer->pop();
		}
	}

	template<typename U = T, typename std::enable_if_t<is_disallowed_ptr_v<U>, bool> = false>
	void Push(U item)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_push_pointer->push(item);
		m_condition.notify_one();
	}

	template<typename U = T, typename std::enable_if_t<!is_disallowed_ptr_v<U>, bool> = false>
	void Push(const T& item)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_push_pointer->push(item);
		m_condition.notify_one();
	}

	template<typename U = T, typename std::enable_if_t<!is_disallowed_ptr_v<U>, bool> = false>
	void Push(U&& item)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_push_pointer->push(std::move(item));
		m_condition.notify_one();
	}

	T Pop()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condition.wait(lock, [this]() { return !m_pop_pointer->empty(); });

		// Swap queues if necessary  
		if (m_pop_pointer->empty() && !m_push_pointer->empty())
		{
			std::swap(m_push_pointer, m_pop_pointer);
		}

		T item = std::move(m_pop_pointer->top());
		m_pop_pointer->pop();
		return item;
	}

	bool Empty() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_push_pointer->empty() && m_pop_pointer->empty();
	}

	std::size_t Size() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_push_pointer->size() + m_pop_pointer->size();
	}

private:
	mutable std::mutex m_mutex;
	std::condition_variable m_condition;
	QueueType* m_push_pointer;
	QueueType* m_pop_pointer;

	// real data  
	QueueType m_queue1;
	QueueType m_queue2;
};  
} // namespace queue  
} // namespace core