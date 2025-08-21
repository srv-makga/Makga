#pragma once

#include "ai.h"

class AIAggressive : public AI
{
public:
	AIAggressive(std::shared_ptr<Actor> _actor);
	virtual ~AIAggressive();

	void initialize() override;
	Status update() override;
};