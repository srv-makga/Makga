#pragma once

#include "ai_base.h"

class Actor;

class ActorAI
{
public:
	ActorAI(Actor* _actor, fb::eAiType _type);
	virtual ~ActorAI();

	void Initialize();
	void OnUpdate();

	bool Create();

	eAiType Type() const;

private:
	Actor* m_actor;
	core::ai::Node* m_node;
};