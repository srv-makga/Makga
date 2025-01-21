#pragma once

#include "ai_base.h"

class AIAggressive : public core::ai::Node
{
public:
	AIAggressive(std::shared_ptr<Actor> _actor);
	virtual ~AIAggressive();

	void initialize() override;
	void terminate(Status s) override;
	Status update() override;

	const core::ai::Node::Ptr root() { return m_root; }

private:
	std::shared_ptr<Actor> m_actor;
	Node::Ptr m_root = nullptr;
};