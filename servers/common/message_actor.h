#pragma once

#include <memory>
#include <functional>
#include <tuple>

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

	virtual void PushMessage(std::unique_ptr<Message> message) = 0;
	virtual void ProcessMessages(std::unique_ptr<Message> message) = 0;

	void OnUpdate(float delta_time) {}
	Id GetId() const { return id_; }

protected:
	const Id id_;
};

struct Message
{
	enum class Type : uint16_t
	{
		Unknown = 0,
		Move,
	};

	MessageActor::Id send_id = 0; // 메시지 발신자
	MessageActor::Id id = 0; // 메시지 수신자
	Type type = Type::Unknown;

	Message(MessageActor::Id sender, MessageActor::Id receiver, Type msg_type)
		: send_id(sender), id(receiver), type(msg_type) {
	}
	virtual ~Message() = default;
	virtual void Execute(MessageActor* actor) = 0;
	void operator()(MessageActor* actor) { Execute(actor); }
};

template<typename... Args>
struct MessageT : public Message
{
	std::tuple<Args...> data;

	MessageT(MessageActor::Id sender, MessageActor::Id receiver, Type msg_type, Args... args)
		: Message(sender, receiver, msg_type), data(std::forward<Args>(args)...)
	{
	}

	void Execute(MessageActor* actor) override
	{
		// ProcessMessages도 이 타입을 처리할 수 있어야 함
		actor->ProcessMessages(std::make_unique<MessageT<Args...>>(*this));
	}
};

struct MessageLambda : public Message
{
	std::function<void(MessageActor*)> func;

	MessageLambda(MessageActor::Id sender, MessageActor::Id receiver, std::function<void(MessageActor*)> f)
		: Message(sender, receiver, Type::Unknown), func(f)
	{
	}

	void Execute(MessageActor* actor) override
	{
		if (nullptr != func)
		{
			func(actor);
		}
	}
};