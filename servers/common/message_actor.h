#pragma once

import makga.lib.doublequeue;
import makga.network.jobhandler;

class MessageActor
{
public:
	using Job = std::shared_ptr<makga::network::Job>;

public:
	MessageActor(ActorId_t actor_id);
	virtual ~MessageActor();

	bool Initialize();
	void Finalize();

	void PushMessage(Job job);

protected:
	virtual Job PopMessage();

private:
	ActorId_t actor_id_;
	makga::lib::DoubleQueue<Job> message_queue_;
};