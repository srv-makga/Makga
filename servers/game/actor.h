#pragma once

#include "bt_node.h"

import makga.math.vector3;

class Actor
{
	friend class AIController;

public:
	Actor();
	virtual ~Actor();

	bool Initialize();
	void Finalize();

	bool SetMovePosition(float x, float y, float z);
	bool SetMovePosition(const makga::math::Vector3& _vector);

	std::shared_ptr<Actor> FindTarget();
	bool ChangeTarget(std::shared_ptr<Actor> target);

	bool IsDead() const;
	bool HasTarget() const;

	makga::AIType GetAIType() const;
};