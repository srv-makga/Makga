#pragma once

#include "../Common/data_table.h"

class DataManager : public DataTable
{
public:
	DataManager() = default;

	bool Load() override;

	const ActorBasicTable* FindActorBasicTable(MonsterIdx_t _idx) const;
	const CharacterBasicTable* FindCharacterBasicTable(MonsterIdx_t _idx) const;
	const MonsterBasicTable* FindMonsterBasicTable(MonsterIdx_t _idx) const;

public:
	std::map<MonsterIdx_t, MonsterBasicTable*> m_monster_basic_table;
	std::map<TableIdx_t, CharacterBasicTable*> m_character_basic_table;
};