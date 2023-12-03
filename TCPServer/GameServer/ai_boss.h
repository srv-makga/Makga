#pragma once

#include "ai_base.h"

class Actor;

class AIBoss : public core::ai::Node
{
public:
	AIBoss(Actor* _actor);
	~AIBoss() {}

	void initialize() override;
	void terminate(Status s) override;
	Status update() override;

	const core::ai::Node::Ptr root() { return m_root; }

private:
	Actor* m_actor;
	Node::Ptr m_root = nullptr;
};