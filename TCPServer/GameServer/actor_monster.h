#pragma once

#include "actor.h"

class Monster : public Actor, public core::ObjectPool<Monster*>
{
public:
	Monster();
	~Monster();

	void Initialize() override;
	void Finallize() override;

	void OnUpdate() override;
};