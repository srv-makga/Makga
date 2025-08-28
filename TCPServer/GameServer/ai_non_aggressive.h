#pragma once

#include "ai.h"

class AINonAggressive : public AI, public std::enable_shared_from_this<AINonAggressive>
{
public:
	AINonAggressive(std::shared_ptr<Actor> _actor);
	virtual ~AINonAggressive();

	void initialize() override;
	Status update() override;
};