#include "pch.h"
#include "ReinforceManager.h"
#include "game_header.h"
#include "data_manager.h"
#include "item_object_base.h"

void ReinforceManager::Initialize()
{
}

Result_t ReinforceManager::Run(ItemObjectBase* _main, ItemObjectBase* _materials)
{
	if (nullptr == _main)
	{
		return eResult_ItemNotFound;
	}

	if (_main->Reinforce() >= DATA_MANAGER.ItemReinforce().max_reinforce)
	{
		return ;
	}

	


	return eResult_Success();
}
