#pragma once

#include "actor.h"
#include "actor_ai.h"

class ActorAI;

class Monster : public Actor, public core::ObjectPool<Monster*>
{
public:
	Monster();
	~Monster();

	void Initialize() override;
	void Finallize() override;

	bool SetTable(ActorBasicTable* _table) override;

	void OnUpdate() override;

	fb::eActorType Type() const override { return fb::eActorType_Monster; }

private:
	const ActorBasicTable* m_basic_table;

	PositionXY m_target_position;

	ActorAI* m_ai;
};