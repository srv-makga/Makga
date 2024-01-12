#pragma once

#include "actor.h"

class Gadget : public Actor, private core::ObjectPool<Gadget*>
{
public:
	Gadget(ActorUid_t _uid);
	virtual ~Gadget();

	void Initialize() override;
	void Finallize() override;

	void OnUpdate() override;

public:
	static Gadget* Rental()
	{
		return Pop();
	}
	static void Return(Gadget* _actor)
	{
		Push(_actor);
	}
};