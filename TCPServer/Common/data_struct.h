#pragma once

#include "common_struct.h"
#include <string>
#include <vector>
#include <map>

struct SystemValue_Actor
{
	
	Distance_t max_around_distance = 30.f; // appear, disappear, move를 계산할 거리
	Count_t max_move_list = 40; // appear, disappear, move를 알릴 각각 최대 유저 수
	Distance_t character_sight = 20.f;
};

struct SystemValueInfo
{
	SystemValue_Actor actor;

	Time_t interraction_wait_time = 30;

	Gold_t resurrection_need_currency = 1000;

	Reinforce_t max_reinforce = 15;
};

struct TerrainInfo
{
	TerrainIdx_t idx = 0;
	String8 name;
	String8 nav_mesh_name;

	CoordPoint_t left_top;
	CoordPoint_t right_bottom;
};

struct ItemTableInfo
{
	ItemIdx_t item_idx;
	StackCount_t stack_count;
	fb::eItemType item_type;
};

struct ItemRefineInfo
{
	StackCount_t need_money = 100000;
	Rate_t success_rate = 1.0f;
};

struct ActorBasicTable
{
	eActorType actor_type = eActorType_None;
	eAiType ai_type;
};

struct MonsterBasicTable : public ActorBasicTable
{
};

struct MonsterRouteTable
{
	std::vector<Vector_t> route_list;
};

struct MonsterSpawnTable
{
	MonsterCreateOption option;

	Tick_t respawn_cycle_tick; // 생성 간격
	Count_t max_count; // 최대 생성
	Count_t cur_count; // 현재 생성
	std::pair<Count_t, Count_t> group_count; // 한번에 생성될 최소, 최대

	std::vector<Vector_t> spawn_pos;
	Distance_t spawn_distance;
	TableIdx_t route_idx;

};