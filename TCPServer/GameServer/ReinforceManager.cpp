#include "pch.h"
#include "ReinforceManager.h"
#include "game_header.h"
#include "data_manager.h"
#include "item_property.h"
#include "item_object_base.h"

void ReinforceManager::Initialize()
{
}

Result_t ReinforceManager::Run(ItemObjectBase* _main, ItemObjectBase* _materials)
{
	if (nullptr == _main)
	{
		DLOG_ERROR << "Main item" << LOG_RESULT(eResult_ItemNotFound);
		return eResult_ItemNotFound;
	}

	if (nullptr == _materials)
	{
		RETURN_RESULT("Materials item", eResult_ItemNotFound);
	}

	if (_main->Reinforce() >= DATA_MANAGER.ItemReinforce().max_reinforce)
	{
		RETURN_RESULT("Max reinforce", eResult_ItemAlreadyMaxReinforce);
	}

	// get reinforce 
	_main->Property()->


	return eResult_Success;
}
