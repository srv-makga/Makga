#pragma once

import makga.lib.doublequeue;

class MessageActor
{
public:
	MessageActor();
	virtual ~MessageActor();

	bool Initialize();
	void Finalize();

	void PushMessage(makga::lib::DoubleQueue::Job_t&& job);

protected:
	virtual makga::lib::DoubleQueue::Job_t PopMessage();

private:
	makga::lib::DoubleQueue message_queue_;
};