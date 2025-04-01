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

struct StatAttackDefence
{
	Attack_t attack = 0; // 공격력
	Defence_t defence = 0; // 방어력
};

struct StatRates
{
	Rate_t rate = 0.f; // 발동률
	Rate_t resist_rate = 0.f; // 회피율
};

struct Stat
{
	HpMpT hp_mp;
	HpMpT hp_mp_gen;

	StatAttackDefence phisical; // 물리
	StatAttackDefence magical; // 마법
	
	StatRates hit; // 명중률
	StatRates critical; // 크리티컬 확률
	StatRates stun; // 스턴 확률
	StatRates blocking; // 블로킹 확률
};

struct MonsterBasicTable : public ActorBasicTable
{
	eActorType actor_type = eActorType_None;

	MonsterIdx_t idx = 0; // 몬스터 인덱스
	TableIdx_t group_idx = 0; // 그룹 인덱스

	Level_t level = 1; // 레벨

	Distance_t attack_range = 0; // 공격 범위
	Distance_t sight = 0; // 시야

	Speed_t spped_attack = 0; // 공격 속도
	Speed_t speed_move = 0; // 이동 속도

	HpMpT hp_mp;
	HpMpT hp_mp_gen;

	StatAttackDefence phisical; // 물리
	StatAttackDefence magical; // 마법

	StatRates hit; // 명중률
	StatRates critical; // 크리티컬 확률
	StatRates stun; // 스턴 확률
	StatRates blocking; // 블로킹 확률

	fb::eAiType ai_type = fb::eAiType_NonAggressive; // AI 타입

	TableIdx_t spawn_idx = 0;
};

struct NpcBasicTable : public ActorBasicTable
{
};

struct GadgetBasicTable : public ActorBasicTable
{
};

struct PetBasicTable : public ActorBasicTable
{
};

struct RouteTable
{
	std::vector<Vector_t> route_list;
};

struct SpawnTable
{
	TableIdx_t idx = 0;

	MonsterCreateOption option;

	Tick_t respawn_cycle_tick = 0; // 생성 간격
	Count_t max_count = 0; // 최대 생성
	std::pair<Count_t, Count_t> group_count; // 한번에 생성될 최소, 최대 수량

	std::vector<fb::PositionT> spawn_pos; // 스폰 중심 위치
	Distance_t spawn_distance = 0; // 스폰시 위치
	TableIdx_t route_idx = 0;

};