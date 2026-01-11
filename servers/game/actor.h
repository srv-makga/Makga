#pragma once

#include "bt_actor.h"
#include "bt_node.h"

class Actor : public BtActor
{
	friend class AIController;

public:
	Actor();
	virtual ~Actor();

	bool Initialize();
	void Finalize();



	bool IsDead() const;
};
