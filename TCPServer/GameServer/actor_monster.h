#pragma once

#include "actor.h"
#include "ai_base.h"

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

	core::ai::Node* m_ai;
};