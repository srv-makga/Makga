#pragma once

#include "common_struct.h"
#include <string>
#include <vector>
#include <map>

struct SystemValue_Actor
{
	
	Distance_t max_around_distance = 30.f; // appear, disappear, move�� ����� �Ÿ�
	Count_t max_move_list = 40; // appear, disappear, move�� �˸� ���� �ִ� ���� ��
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
	Attack_t attack = 0; // ���ݷ�
	Defence_t defence = 0; // ����
};

struct StatRates
{
	Rate_t rate = 0.f; // �ߵ���
	Rate_t resist_rate = 0.f; // ȸ����
};

struct Stat
{
	HpMpT hp_mp;
	HpMpT hp_mp_gen;

	StatAttackDefence phisical; // ����
	StatAttackDefence magical; // ����
	
	StatRates hit; // ���߷�
	StatRates critical; // ũ��Ƽ�� Ȯ��
	StatRates stun; // ���� Ȯ��
	StatRates blocking; // ���ŷ Ȯ��
};

struct MonsterBasicTable : public ActorBasicTable
{
	eActorType actor_type = eActorType_None;

	MonsterIdx_t idx = 0; // ���� �ε���
	TableIdx_t group_idx = 0; // �׷� �ε���

	Level_t level = 1; // ����

	Distance_t attack_range = 0; // ���� ����
	Distance_t sight = 0; // �þ�

	Speed_t spped_attack = 0; // ���� �ӵ�
	Speed_t speed_move = 0; // �̵� �ӵ�

	HpMpT hp_mp;
	HpMpT hp_mp_gen;

	StatAttackDefence phisical; // ����
	StatAttackDefence magical; // ����

	StatRates hit; // ���߷�
	StatRates critical; // ũ��Ƽ�� Ȯ��
	StatRates stun; // ���� Ȯ��
	StatRates blocking; // ���ŷ Ȯ��

	fb::eAiType ai_type = fb::eAiType_NonAggressive; // AI Ÿ��

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

	Tick_t respawn_cycle_tick = 0; // ���� ����
	Count_t max_count = 0; // �ִ� ����
	std::pair<Count_t, Count_t> group_count; // �ѹ��� ������ �ּ�, �ִ� ����

	std::vector<fb::PositionT> spawn_pos; // ���� �߽� ��ġ
	Distance_t spawn_distance = 0; // ������ ��ġ
	TableIdx_t route_idx = 0;

};