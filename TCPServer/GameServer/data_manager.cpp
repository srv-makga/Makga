#include "pch.h"
#include "data_manager.h"

bool DataManager::Load()
{
	return true;
}

const ActorBasicTable* DataManager::FindActorBasicTable(MonsterIdx_t _idx) const
{
	return nullptr;
}

const MonsterBasicTable* DataManager::FindMonsterBasicTable(MonsterIdx_t _idx) const
{
	auto iter = m_monster_basic_table.find(_idx);
	if (m_monster_basic_table.end() != iter)
	{
		return iter->second;
	}

	return nullptr;
}
