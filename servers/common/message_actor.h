#pragma once

#include <memory>
#include <functional>
#include <tuple>
#include "define.h"

import makga.lib.mpsc_queue;

struct Message;

class MessageActor
{
public:
	using Id = uint64_t;

	MessageActor(Id id)
		: id_(id) {
	}
	virtual ~MessageActor() = default;

	virtual bool IsValid() const { return false; }
	virtual void OnUpdate(float delta_time) {}

	virtual void PushMessage(std::unique_ptr<Message> message) = 0;
	virtual void ProcessMessages(std::unique_ptr<Message> message) {}

	Id GetId() const { return id_; }

protected:
	const Id id_;
};

struct Message
{
	MessageActor::Id send_id = 0;
	MessageActor::Id id = 0;
	makga::MessageType type;

	Message(MessageActor::Id sender, MessageActor::Id receiver, makga::MessageType msg_type)
		: send_id(sender), id(receiver), type(msg_type) {
	}
	virtual ~Message() = default;
	virtual void Execute(MessageActor* actor) = 0;
	virtual std::unique_ptr<Message> Clone() const = 0;
	void operator()(MessageActor* actor) { Execute(actor); }
};

template<typename... Args>
struct MessageT : public Message
{
	std::tuple<Args...> data;

	MessageT(MessageActor::Id sender, MessageActor::Id receiver, makga::MessageType msg_type, Args... args)
		: Message(sender, receiver, msg_type), data(std::forward<Args>(args)...)
	{
	}

	void Execute(MessageActor* actor) override
	{
		// ProcessMessages에 각 타입별 처리가 구현되어 있어야 함
		actor->ProcessMessages(std::make_unique<MessageT<Args...>>(*this));
	}

	std::unique_ptr<Message> Clone() const override
	{
		return std::make_unique<MessageT<Args...>>(*this);
	}
};

template<typename T>
struct MessageLambda : public Message
{
	std::function<void(MessageActor*)> func;

	MessageLambda(MessageActor::Id sender, MessageActor::Id receiver, makga::MessageType msg_type, std::function<void(MessageActor*)> f)
		: Message(sender, receiver, msg_type), func(f)
	{
	}

	void Execute(MessageActor* actor) override
	{
		if (nullptr != func)
			func(actor);
	}

	std::unique_ptr<Message> Clone() const override
	{
		return std::make_unique<MessageLambda<T>>(send_id, id, type, func);
	}
};