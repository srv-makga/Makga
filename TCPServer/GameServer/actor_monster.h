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

	void OnUpdate() override;

private:
	PositionXY m_target_position;

	core::ai::Node* m_ai;
};