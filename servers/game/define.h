#pragma once

#include "../common/define.h"

enum class MessageType
{
	None = 0,
	GridAddActor = 1,
	GridRemoveActor = 2,
	GridBroadcastMove = 3,
	ActorMove = 4,
	SetTarget = 5,
	SetLeader = 6,
	SetOwner = 7,
	DeleteActor = 8,
	Max
};