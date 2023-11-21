#pragma once

#include "ai_aggressive.h"
#include "ai_non_aggressive.h"
#include "ai_boss.h"

class Actor;

class BehaviorTree
{
public:
	BehaviorTree(Actor* _actor);
	virtual ~BehaviorTree();

	void Initialize();
	void OnUpdate();
	
	eAiType Type() const;
	void ChangeType(eAiType _type);

private:
	Actor* m_actor;
	core::ai::Node* m_node;
};