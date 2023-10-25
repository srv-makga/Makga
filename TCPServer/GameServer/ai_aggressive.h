#pragma once

#include "base_ai.h"

class Actor;

class AIAggressive : public core::ai::Node
{
public:
	AIAggressive(Actor* _actor);
	~AIAggressive() {}

	void initialize() override;
	void terminate(Status s) override;
	Status update() override;

	const core::ai::Node::Ptr root() { return m_root; }

private:
	Actor* m_actor;
	Node::Ptr m_root = nullptr;
};