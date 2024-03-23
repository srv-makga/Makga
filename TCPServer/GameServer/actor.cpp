#include "pch.h"
#include "actor.h"
#include "terrain.h"
#include "user.h"
#include "data_manager.h"

void Actor::SendDelete(Actor* _to_actor, eActorMoveEffect _effect)
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
	std::vector offset = { fb::CreateActorDisAppear(fbb, Uid(), _effect) };
	fbb.Finish(fb::server::CreateRecv_ActorDisappear(fbb, fbb.CreateVector(offset)));
	user->Send(fb::server::RecvPid_ActorDisappear, fbb);
}

void Actor::SendDelete(const std::vector<Actor*>& _to_actors, eActorMoveEffect _effect)
{
	if (true == _to_actors.empty())
	{
		return;
	}

	CREATE_FBB(fbb);
	std::vector offset = { fb::CreateActorDisAppear(fbb, Uid(), _effect) };
	fbb.Finish(fb::server::CreateRecv_ActorDisappear(fbb, fbb.CreateVector(offset)));

	for (Actor* actor : _to_actors)
	{
		User* user = actor->OwnerUser();
		if (nullptr == user)
		{
			return;
		}

		user->Send(fb::server::RecvPid_ActorDisappear, fbb);
	}
}

void Actor::SendDelete(const std::unordered_set<Actor*>& _to_actors, eActorMoveEffect _effect)
{
	if (true == _to_actors.empty())
	{
		return;
	}

	CREATE_FBB(fbb);
	std::vector offset = { fb::CreateActorDisAppear(fbb, Uid(), _effect) };
	fbb.Finish(fb::server::CreateRecv_ActorDisappear(fbb, fbb.CreateVector(offset)));

	for (Actor* actor : _to_actors)
	{
		User* user = actor->OwnerUser();
		if (nullptr == user)
		{
			return;
		}

		user->Send(fb::server::RecvPid_ActorDisappear, fbb);
	}
}

void Actor::MulticastHpMp()
{
	CREATE_FBB(fbb);
	fbb.Finish(fb::server::CreateRecv_CharacterHpMp(fbb,
		Uid(),
		fb::CreateHpMp(fbb, &m_hp_mp)
	));

	Multicast(fb::server::RecvPid_CharacterHpMp, fbb);
}

void Actor::Multicast(fb::server::RecvPid _pid, flatbuffers::FlatBufferBuilder& _builder)
{
	Terrain* terrain = getTerrain();
	if (nullptr == terrain)
	{
		return;
	}

	ActorList actor_list;
	if (false == terrain->AroundList(Position(), SYSTEM.actor.max_around_distance, eActorSearchFilter::FilterCharacter, actor_list))
	{
		return;
	}

	for (Actor* actor : actor_list)
	{
		User* user = actor->OwnerUser();
		if (nullptr == user)
		{
			continue;
		}

		user->Send(_pid, _builder);
	}
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
