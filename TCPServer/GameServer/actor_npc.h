#pragma once

#include "actor.h"

class Npc : public Actor, private core::ObjectPool<Npc*>
{
public:
	Npc(ActorUid_t _uid);
	virtual ~Npc();

	void Initialize() override;
	void Finallize() override;

	void OnUpdate() override;

public:
	static Npc* Rental()
	{
		return Pop();
	}
	static void Return(Npc* _actor)
	{
		Push(_actor);
	}
};