#pragma once

#include "ai.h"

class AIBoss : public AI, public std::enable_shared_from_this<AIBoss>
{
public:
	AIBoss(std::shared_ptr<Actor> _actor);
	virtual ~AIBoss();

	void initialize() override;
	Status update() override;
};