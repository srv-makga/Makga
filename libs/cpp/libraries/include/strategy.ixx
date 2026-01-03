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

	virtual ~StrategyContext()
	{
		strategy_ = nullptr;
	}
	
	void SetStrategy(std::shared_ptr<Strategy<T>> strategy)
	{
		strategy_ = strategy;
	}

	void Execute()
	{
		if (nullptr != strategy_)
		{
			strategy_->Execute();
		}
	}

protected:
	std::shared_ptr<Strategy<T>> strategy_;
};
} // namespace makga::lib