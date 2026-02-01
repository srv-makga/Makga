#pragma once

export module makga.lib.message_broker;

import <vector>;
import <unordered_map>;
import <functional>;
import <atomic>;
import makga.lib.mpsc_queue;
import makga.lib.lock;

namespace makga::lib {
export template<typename Topic, typename Message>
class MessageBroker
{
public:
	using CallBack = std::function<void(const Message&)>;
	using SubscriptionId = uint64_t;

	MessageBroker() = default;
	virtual ~MessageBroker() = default;

	void Publish(const Topic& topic, const Message& message)
	{
		message_queue_.Enqueue({ topic, message });
	}

	SubscriptionId Subscribe(const Topic& topic, CallBack& callback)
	{
		SubscriptionId id = next_subscription_id_.fetch_add(1, std::memory_order_relaxed);

		{
			WriteLock lock(mutex_);
			subscribers_[topic].emplace_back(id, callback);
		}

		return id;
	}

	bool Unsubscribe(SubscriptionId id)
	{
		bool is_remove = false;

		WriteLock lock(mutex_);
		for (auto& [topic, subscriber] : subscribers_)
		{
			auto iter = subscriber.find(id);
			if (subscriber.end() != iter)
			{
				subscriber.erase(iter);
				is_remove = true;
				break;
			}
		}

		return is_remove;
	}

	// @brief 메시지 큐에 쌓여있는 모든 메시지 처리
	void ProcessMessages()
	{
		std::pair<Topic, Message> item;
		while (message_queue_.Dequeue(item))
		{
			ReadLock lock(mutex_);
			auto iter = subscribers_.find(item.first);
			if (subscribers_.end() == iter)
			{
				continue;
			}
			for (const auto& [id, callback] : iter->second)
			{
				callback(item.second);
			}
		}
	}

protected:
	MPSCQueue<std::pair<Topic, Message>> message_queue_;

	SharedMutex mutex_;
	std::unordered_map<Topic, std::unordered_map<SubscriptionId, CallBack>> subscribers_;

	std::atomic<SubscriptionId> next_subscription_id_ = 0;
};
} // namespace makga::lib