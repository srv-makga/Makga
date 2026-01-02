module;

export module makga.lib.pattern.strategy;

import <memory>;
import <shared_mutex>;

export namespace makga::lib {
export template<typename T>
// @detail Strategy를 상속받아 ExecuteImpl() 함수를 구현해야 한다
class Strategy
{
public:
	Strategy() = default;
	Strategy(const Strategy<T>&) = delete;
	Strategy(Strategy<T>&&) = delete;
	Strategy<T>& operator=(const Strategy<T>&) = delete;
	Strategy<T>& operator=(Strategy<T>&&) = delete;
	virtual ~Strategy() = default;

	void Execute()
	{
		static_cast<T*>(this)->ExecuteImpl();
	}
};

export template<typename T>
class StrategyContext
{
public:
	StrategyContext(std::shared_ptr<Strategy<T>> strategy)
		: strategy_(strategy)
	{
	}

	virtual StrategyContext::~StrategyContext()
	{
		strategy_ = nullptr;
	}
	
	void SetStrategy(std::shared_ptr<Strategy<T>> strategy)
	{
		strategy_ = strategy;
	}

	void Execute()
	{
		std::shared_ptr<Strategy<T>> strategy_copy;
		{
			std::shared_lock lock(mutex_);
			strategy_copy = strategy_;
		}
		if (nullptr != strategy_copy)
		{
			strategy_copy->Execute();
		}
	}

protected:
	std::shared_ptr<Strategy<T>> strategy_;
};
} // namespace makga::lib