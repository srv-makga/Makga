#pragma once

#include "actor.h"
#include "actor_ai.h"

class ActorAI;

class Monster : public Actor, private core::ObjectPool<Monster*>
{
public:
	Monster();
	virtual ~Monster();

	void Initialize() override;
	void Finallize() override;

	bool SetTable(ActorBasicTable* _table) override;

	void OnUpdate() override;

	Actor* FindTarget() override;

public:
	fb::eActorType Type() const override { return fb::eActorType_Monster; }

public:
	static Monster* Rental()
	{
		return Pop();
	}
	static void Return(Monster* _actor)
	{
		Push(_actor);
	}

private:
	const ActorBasicTable* m_basic_table;

	PositionXY m_target_position;

	ActorAI* m_ai;
};