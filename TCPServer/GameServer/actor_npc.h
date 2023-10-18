#pragma once

#include "actor.h"

class Npc : public Actor, public core::ObjectPool<Npc*>
{
public:
	Npc();
	~Npc();

	void Initialize() override;
	void Finallize() override;

	void OnUpdate() override;
};