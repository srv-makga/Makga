#pragma once

#include "actor.h"

class Monster : public Actor
{
public:
	Monster();
	virtual ~Monster();

public: // virtual
	void OnUpdate(float delta_time) override;
	makga::AIType GetAIType() const override;
	makga::ActorType GetActorType() const override;
};