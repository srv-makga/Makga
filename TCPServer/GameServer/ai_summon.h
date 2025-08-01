#pragma once

#include "ai.h"

class AISummon : public AI, public std::enable_shared_from_this<AISummon>
{
public:
	AISummon(std::shared_ptr<Actor> _actor);
	virtual ~AISummon();

	void initialize() override;
	Status update() override;
};