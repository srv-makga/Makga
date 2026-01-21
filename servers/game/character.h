#pragma once

#include "actor.h"

class Character : public Actor
{
public:
	Character();
	virtual ~Character();

public: // virtual
	void OnUpdate(float delta_time) override;
	makga::AIType GetAIType() const override;
	makga::ActorType GetActorType() const override;
};