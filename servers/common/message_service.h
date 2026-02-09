#pragma once

#include "define.h"
#include "message_actor.h"

// @brief 메시지 처리 서비스
// @detail 여러 스레드에서 메시지를 받아 처리하는 서비스
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
	virtual void ProcessMessages();

protected:
	std::atomic<bool> is_running_;
	MessageQueue message_queue_;
};