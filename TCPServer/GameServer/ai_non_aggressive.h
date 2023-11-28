#pragma once

#include "ai_base.h"

class AINonAggressive : public core::ai::Node
{
public:
	AINonAggressive(Actor* _actor);
	~AINonAggressive();

	void initialize() override;
	void terminate(Status s) override;
	Status update() override;

	const core::ai::Node::Ptr root() { return m_root; }

private:
	Actor* m_actor;
	Node::Ptr m_root = nullptr;
};