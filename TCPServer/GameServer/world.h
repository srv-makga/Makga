#pragma once

#include "../Core/singleton.hpp"
#include "../Common/utility.h"
#include "data_manager.h"
#include "actor_manager.h"
#include "terrain_manager.h"
#include "user_manager.h"

class World final : public core::pattern::Singleton<World>
{
public:
	World() = default;
	~World() = default;

	bool Initialize()
	{
		if (false == data.Initialize("./"))
		{
			return false;
		}

		// @todo terrain range
		return true;
	}

	std::shared_ptr<Actor> GetActor(ActorUid_t _uid)
	{
		return actor.FindActor(_uid);
	}
	

public:
	DataManager data;
	ActorManager actor;
	TerrainManager terrain;
	UserManager user;

	Utility utility;
};

static World& GetWorld()
{
	return World::Instance();
}