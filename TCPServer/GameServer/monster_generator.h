#pragma once

#include "game_header.h"

class MonsterGenerator
{
public:
	MonsterGenerator();
	virtual ~MonsterGenerator();

	void SetTable(const MonsterSpawnTable* _table);

private:
	inline static const MonsterSpawnTable s_empty_table{};
	const MonsterSpawnTable* m_spawn_table;
};