#include "stdafx.h"
#include "message_actor.h"
#include "iocp_server.h"

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

void MessageActor::PushMessage(Job job)
{
	message_queue_.Push(job);
}

MessageActor::Job MessageActor::PopMessage()
{
	return message_queue_.Pop();
}