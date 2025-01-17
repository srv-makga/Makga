#pragma once

#include "actor.h"

class Npc : public Actor
{
public:
	Npc();
	virtual ~Npc();

	void Initialize() override;
	void Finalize() override;

	void OnUpdate() override;
};