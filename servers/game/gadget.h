#pragma once

#include "actor.h"

class Gadget : public Actor
{
public:
	Gadget();
	virtual ~Gadget();

public: // virtual
	//virtual void OnUpdate(float delta_time);
	makga::AIType GetAIType() const override;
	makga::ActorType GetActorType() const override;
};