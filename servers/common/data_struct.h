#pragma once

#include "pch.h"

// 공용 구조체

struct StatEffect
{
	StatType type;
	float value = 0.0f;
	bool is_percentage = false;
};

struct AbilityEffect
{
	AbilityType type;
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

struct SkillLevelTable
{
	SkillLevel level = 0;
	std::vector<StatEffect> stats;
	std::vector<AbilityEffect> abilities;
};

struct SkillTable
{
	SkillIdx idx = 0;

	Tick casting_time = 0;
	Tick cool_time = 0;

	int32_t hit_count = 0;	// 적용 최대 대상

	std::map<SkillLevel, SkillLevelTable> level_table;
};