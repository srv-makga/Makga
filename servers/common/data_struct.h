#pragma once

#include "pch.h"

// 공용 구조체

struct StatEffect
{
	makga::StatType type;
	float value = 0.0f;
	bool is_percentage = false;
};

struct AbilityEffect
{
	makga::AbilityType type;
	float value = 0.0f; // 효과값
	Tick duration = 0; // 지속시간
};

// 데이터 테이블 구조체

struct SystemTable
{
};



struct MapTable
{
	TerrainIdx idx = 0;

};


struct MonsterTable
{
	MonsterIdx idx = 0;

	makga::AIType ai_type = makga::AIType::AIType_NonAggressive;
};

struct ItemRandomOptionTable
{
	int32_t option_idx = 0;
	int32_t option_value = 0;
};

struct ItemTable
{
	ItemIdx idx = 0;

	// 메인 능력치
	// 서브 능력치(랜덤)
	// 최대 강화 수치
	RefineLevel max_refine = 0;
	// 최대 인첸트 갯수
	int32_t max_enchant_count = 0;
};

struct BuffTable
{
	SkillIdx idx = 0;

	Tick duration_time = 0;			// 지속 시간
	std::vector<StatEffect> stats;
	std::vector<AbilityEffect> abilities;
};;

struct SkillLevelTable
{
	SkillLevel level = 0;

	std::vector<StatEffect> stats;
	std::vector<AbilityEffect> abilities;

	std::vector<BuffTable> buff_tables;
};

struct SkillTable
{
	SkillIdx idx = 0;

	Prob success_rate = max_prob; // 스킬 성공 확률

	makga::CostType cost_type = makga::CostType_Gold;
	int32_t cost_value = 0;

	int32_t casting_group = 0; // 같은 group 내 스킬들은 동시에 시전 불가
	Tick casting_time = 0;

	int32_t cool_group = 0; // 같은 group 내 스킬들은 동시에 쿨타임 적용
	Tick cool_time = 0;

	int32_t hit_count = 0;				// 적용 최대 대상
	int32_t max_duplicate_count = 0;	// 최대 중첩 수
	int32_t max_overlap_count = 0;		// 최대 동시 적용 수

	bool is_force = false;				// 강제로 적용
	bool is_only_instance = false;		// 인스턴스 던전에서만 사용 가능

	std::map<SkillLevel, SkillLevelTable> level_table; // 스킬 레벨별 테이블
};