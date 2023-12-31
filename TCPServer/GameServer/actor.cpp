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
