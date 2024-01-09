#pragma once

#include "game_header.h"

class MonsterGenerator
{
public:
	MonsterGenerator();
	virtual ~MonsterGenerator();

	void AddSpawnTable(const MonsterSpawnTable* _table);


	void OnUpdate();

protected:
	static Monster* MonsterCreate(const MonsterCreateOption& _option);
private:
	std::vector<std::pair<Tick_t, const MonsterSpawnTable*>> m_spawn_list;
};