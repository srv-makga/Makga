#pragma once

#include "../common/message_service.h"

class GameWorkThread : public MessageService
{
public:
	using ActorList = std::unordered_map<MessageActor::Id, std::unique_ptr<MessageActor>>;
	using MessageQueue = makga::lib::MPSCQueue<std::unique_ptr<Message>>;

	GameWorkThread();
	virtual ~GameWorkThread();

	bool Initialize();
	void Finalize();
	void OnUpdate();
	bool AddActor(std::unique_ptr<MessageActor> actor);
	void EnqueueMessage(std::unique_ptr<Message> message);

protected:
	void ProcessMessages();
	void UpdateActors(float delta_time);

protected:
	std::unordered_map<MessageActor::Id, std::unique_ptr<MessageActor>> actors_;

	std::atomic<bool> is_running_;
	MessageQueue message_queue_;

	Tick last_update_tick_;
};