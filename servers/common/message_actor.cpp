#include "pch.h"
#include "message_actor.h"

MessageActor::MessageActor(ActorId_t actor_id)
	: actor_id_(actor_id)
{
}

MessageActor::~MessageActor()
{
}

bool MessageActor::Initialize()
{
	return true;
}

void MessageActor::Finalize()
{
}

void MessageActor::PushMessage(makga::lib::DoubleQueue::Job_t&& job)
{
	message_queue_.Push(std::move(job));
}

makga::lib::DoubleQueue::Job_t MessageActor::PopMessage()
{
	return message_queue_.Pop();
}