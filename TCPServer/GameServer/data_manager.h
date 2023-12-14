#pragma once

#include "../Common/data_table.h"
#include "../Core/singleton.hpp"

class DataManager : public DataTable, public core::pattern::Singleton<DataManager>
{
public:
	DataManager() = default;

	bool Load() override;
};

#define DATA_MANAGER	DataManager::Instance()
#define SYSTEM			DataManager::Instance().SystemValue()