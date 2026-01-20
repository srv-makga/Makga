#pragma once

import makga.lib.mpsc_queue;

class MessageActor
{
public:
	using Message = std::function<void(void)>;
	using Queue = makga::lib::MPSCQueue<Message>;

public:
	MessageActor();
	virtual ~MessageActor();

	bool Initialize();
	void Finalize();

	void PushMessage(Message message);
	void ProcessMessages();

	Queue& GetMessageQueue();

protected:
	virtual bool PopMessage(Message& message);

	Queue message_queue_;
};