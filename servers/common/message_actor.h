#pragma once

import makga.lib.doublequeue;

// @brief Basic unit class for processing messages based on threads
class MessageActor
{
public:
	MessageActor(ActorId_t actor_id);
	virtual ~MessageActor();

	bool Initialize();
	void Finalize();

	void PushMessage(makga::lib::DoubleQueue::Job_t&& job);

protected:
	virtual makga::lib::DoubleQueue::Job_t PopMessage();

private:
	ActorId_t actor_id_;
	makga::lib::DoubleQueue message_queue_;
};