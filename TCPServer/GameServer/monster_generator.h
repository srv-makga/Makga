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

	void AddSpawnTable(const SpawnTable* _table);


	void OnUpdate();

protected:
	static std::shared_ptr<Actor> MonsterCreate(const MonsterCreateOption& _option);

private:
	Terrain& m_terrain;
	std::map<TableIdx_t, const SpawnTable*> m_spawn_list;
	std::map<TableIdx_t, MonsterSpawnInfo> m_spawn_info;
};