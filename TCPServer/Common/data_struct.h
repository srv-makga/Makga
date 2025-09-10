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

struct SkillAbility
{
	eAbilityType type = eAbilityType_MIN;
	eAbilityTarget target = eAbilityTarget_MIN;
	eAbilityApplyType apply_type = eAbilityApplyType_MIN;
	eAbilityOverlapType overlap_type = eAbilityOverlapType_MIN;
	Time_t duration = 0; // ���� �ð�(0�̸� ��� ȿ��)
};

struct DataTableSkill
{
	SkillIndex_t index = 0;
	SkillLevel_t level = 0;
	eSkillAttackType attack_type = eSkillAttackType_MIN; // ��ų Ÿ��(����, ������)
	eSkillUseType use_type = eSkillUseType_MIN; // ��ų ��� Ÿ��(��Ƽ��, �нú� ��)
	eSkillHitType hit_type = eSkillHitType_MIN; // ���� �ܰ� Ÿ��(����, ����, �޺���)
	eSkillTargetShape target_shape = eSkillTargetShape_MIN; // Ÿ�� ����(����, ����, ���� ��)
	Count_t target_count = 1; // Ÿ�� Ƚ��

	Distance_t cast_distance = 0.f; // ĳ���� ��Ÿ�
	Distance_t apply_distance = 0.f; // ȿ�� ���� ��Ÿ�

	std::vector<SkillAbility> ability_list;
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
	StatRates hit; // ���߷�
	StatRates critical; // ũ��Ƽ�� Ȯ��
	StatRates stun; // ���� Ȯ��
	StatRates blocking; // ���ŷ Ȯ��

	Distance_t attack_range = 0; // ���� ����
	Distance_t sight = 0; // �þ�

	Speed_t spped_attack = 0; // ���� �ӵ�
	Speed_t speed_move = 0; // �̵� �ӵ�

	HpMpT hp_mp;
	HpMpT hp_mp_gen;

	StatAttackDefence phisical; // ����
	StatAttackDefence magical; // ����
};

struct ActorBasicTable
{
	eActorType actor_type = eActorType_None;
	eAiType ai_type = fb::eAiType_NonAggressive; // AI Ÿ��

	Stat stat;
};

struct CharacterBasicTable : public ActorBasicTable
{
	CharIdx_t idx = 0; // ĳ���� �ε���
};

struct MonsterBasicTable : public ActorBasicTable
{
	MonsterIdx_t idx = 0; // ���� �ε���
	TableIdx_t group_idx = 0; // �׷� �ε���

	Level_t level = 1; // ����

	std::vector<std::pair<ItemIdx_t, StackCount_t>> drop_item_list;
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