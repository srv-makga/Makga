#pragma once

#include "ai_base.h"

class Actor;

class ActorAI
{
public:
	ActorAI(Actor* _actor);
	virtual ~ActorAI();

	void Initialize();
	void OnUpdate();

	eAiType Type() const;
	void ChangeType(eAiType _type);

private:
	Actor* m_actor;
	core::ai::Node* m_node;
};