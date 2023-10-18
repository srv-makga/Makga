#pragma once

#include "game_header.h"
#include "manager_base.hpp"

#include "../Core/singleton.hpp"

class ItemProperty;

class ItemManager
	: public ManagerBase<ItemIdx_t, ItemProperty*>
	, public core::pattern::Singleton<ItemManager>
{
};

#define ITEM_MANAGER	ItemManager::Instance()