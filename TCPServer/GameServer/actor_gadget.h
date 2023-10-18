#pragma once

#include "actor.h"

class Gadget : public Actor, public core::ObjectPool<Gadget*>
{
public:
	Gadget();
	~Gadget();

	void Initialize() override;
	void Finallize() override;

	void OnUpdate() override;
};