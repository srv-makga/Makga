module;

#include <vector>
#include <functional>

export module makga.lib.batch_processor;

namespace makga::lib
{
export template<typename T>
class BatchProcessor
{
public:
	using Type = T;
	using ProcessType = std::function<void(std::vector<T>&)>;

	BatchProcessor(std::size_t capacity, ProcessType process)
		: capacity_(capacity), process_(process)
	{
		items_.reserve(capacity_);
	}

	~BatchProcessor()
	{
		Flush();
	}

	void AddItem(T&& item)
	{
		items_.emplace_back(std::move(item));

		if (items_.size() >= capacity_)
		{
			Flush();
		}
	}

	void AddItem(const T& item)
	{
		items_.emplace_back(item);

		if (items_.size() >= capacity_)
		{
			Flush();
		}
	}

	void Flush()
	{
		if (true == items_.empty())
		{
			return;
		}

		process_(items_);
		items_.clear();
	}

private:
	std::vector<T> items_;
	std::size_t capacity_;
	ProcessType process_;
};
} // namespace makga::lib