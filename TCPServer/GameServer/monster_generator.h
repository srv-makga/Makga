#pragma once

#include "game_header.h"

class Monster;
class Terrain;

struct MonsterSpawnInfo
{
	Tick_t tick;
	Count_t count;
};

class MonsterGenerator
{
public:
	MonsterGenerator(Terrain& _terrain);
	virtual ~MonsterGenerator();

	void AddSpawnTable(const MonsterSpawnTable* _table);


	void OnUpdate();

protected:
	static Monster* MonsterCreate(const MonsterCreateOption& _option);

private:
	Terrain& m_terrain;
	std::map<TableIdx_t, const MonsterSpawnTable*> m_spawn_list;
	std::map<TableIdx_t, MonsterSpawnInfo> m_spawn_info;
};