#include "pch.h"
#include "actor.h"
#include "user.h"

void Actor::SendDelete(Actor* _to_actor, eActorDisappearEffect _effect)
{
	if (nullptr == _to_actor)
	{
		return;
	}

	User* user = _to_actor->OwnerUser();
	if (nullptr == user)
	{
		return;
	}

	CREATE_FBB(fbb);
	fbb.Finish(fb::server::CreateRecv_ActorDisappear(fbb, Uid(), _effect));
	user->Send(fb::server::RecvPid_ActorDisappear, fbb);
}

flatbuffers::Offset<fb::ActorAppear> Actor::OffsetActorAppear(FB_BUILDER& _fbb, eActorMoveEffect _effect)
{

	return fb::CreateActorAppear(_fbb, Uid(), OffsetActorInfoBase(_fbb), _effect);
}

flatbuffers::Offset<fb::ActorDisAppear> Actor::OffsetActorDisappear(FB_BUILDER& _fbb, eActorMoveEffect _effect)
{
	return fb::CreateActorDisAppear(_fbb, Uid(), _effect);
}

flatbuffers::Offset<fb::ActorMove> Actor::OffsetActorMove(FB_BUILDER& _fbb)
{
	return fb::CreateActorMove(_fbb, Uid(), fb::CreatePosition(_fbb, &Position()), Speed());
}
