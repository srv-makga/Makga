#pragma once

#include "define.h"
#include "message_actor.h"

// @brief 메시지 서비스. ActorSystem과 별개로, Actor 메시지 외의 일반 메시지를 처리하는 서비스.
class MessageService
{
public:
	using MessageQueue = makga::lib::MPSCQueue<std::unique_ptr<Message>>;

	MessageService();
	virtual ~MessageService();

	bool Initialize();
	void Finalize();

	bool Start();
	void Stop();
	void EnqueueMessage(std::unique_ptr<Message> message);

	virtual void OnUpdate();

protected:
	virtual void ProcessMessages() = 0;

protected:
	std::atomic<bool> is_running_;
	MessageQueue message_queue_;
};
