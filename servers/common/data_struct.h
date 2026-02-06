#pragma once

#include "pch.h"

import makga.math.vector3;

#include <vector>
#include <random>
#include <numeric>
#include <optional>
#include <algorithm>

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

struct TableStruct
{
	TableIdx idx = 0;

	virtual bool LoadFromJson(const rapidjson::Value& json) = 0;
};

struct SystemTable
{
};

struct MapTable : public TableStruct
{
	TerrainIdx terrain_idx = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		return true;
	}
};

struct MonsterTable : public TableStruct
{
	MonsterIdx monster_idx = 0;

	makga::AIType ai_type = makga::AIType::AIType_NonAggressive;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<MonsterIdx>("idx").value_or(0);
		monster_idx = json.TryGet<MonsterIdx>("monster_idx").value_or(0);

		return true;
	}
};

struct ItemRandomOptionTable : public TableStruct
{
	int32_t option_idx = 0;
	int32_t option_value = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<MonsterIdx>("idx").value_or(0);

		return true;
	}
};

struct ItemTable : public TableStruct
{
	ItemIdx item_idx = 0;

	// 메인 능력치
	// 서브 능력치(랜덤)
	// 최대 강화 수치
	RefineLevel max_refine = 0;
	// 최대 인첸트 갯수
	int32_t max_enchant_count = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		item_idx = json.TryGet<ItemIdx>("item_idx").value_or(0);
		max_refine = json.TryGet<RefineLevel>("max_refine").value_or(0);
		max_enchant_count = json.TryGet<int32_t>("max_enchant_count").value_or(0);

		return true;
	}
};

struct BuffTable : public TableStruct
{
	SkillIdx skill_idx = 0;

	Tick duration_time = 0;			// 지속 시간
	std::vector<StatEffect> stats;
	std::vector<AbilityEffect> abilities;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		skill_idx = json.TryGet<SkillIdx>("skill_idx").value_or(0);
		duration_time = json.TryGet<Tick>("duration_time").value_or(0);
		return true;
	}
};;

struct SkillLevelTable : public TableStruct
{
	SkillLevel level = 0;

	std::vector<StatEffect> stats;
	std::vector<AbilityEffect> abilities;

	std::vector<BuffTable> buff_tables;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		level = json.TryGet<SkillLevel>("level").value_or(0);
		return true;
	}
};

struct MovePathTable : public TableStruct
{
	TerrainIdx terrain_idx = 0;
	std::vector<makga::math::Vector3> waypoints;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		terrain_idx = json.TryGet<TerrainIdx>("terrain_idx").value_or(0);
		return true;
	}
};

struct SkillTable : public TableStruct
{
	SkillIdx skill_idx = 0;

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

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		skill_idx = json.TryGet<SkillIdx>("skill_idx").value_or(0);
		cost_value = json.TryGet<int32_t>("cost_value").value_or(0);
		casting_time = json.TryGet<Tick>("casting_time").value_or(0);
		cool_time = json.TryGet<Tick>("cool_time").value_or(0);
		return true;
	}
};

//////////////////////////////////////////////////////
// Spawn 관련 데이터 구조 (데이터 테이블)
/*
  사용법 요약:
  - 한 지점(SpawnPoint)에 여러 SpawnEntry 를 넣어 다양한 몬스터를 스폰할 수 있음.
  - 같은 MonsterIdx 를 여러 SpawnPoint 에 반복해 넣으면 동일 몬스터를 여러 장소에서 스폰.
  - 런타임에서는 SpawnManager 가 아래 구조를 읽어 오브젝트 풀에서 SpawnInstance 를 할당/해제.
*/
//////////////////////////////////////////////////////

struct SpawnOption
{
	Level level = 0;
};

struct SpawnEntry : public TableStruct
{
	MonsterIdx monster_idx = 0; // 스폰할 몬스터 테이블 인덱스

	makga::math::Vector3 position; // 위치
	float direction = 0.0f;

	int32_t min_count = 1; // 선택되었을 때 최소 스폰 수
	int32_t max_count = 1; // 선택되었을 때 최대 스폰 수

	Tick respawn_term = 0; // 리스폰 주기(틱), 0 이면 즉시 스폰
	std::string respawn_time_of_day; // 특정 시간 리스폰(HH:MM:SS), 비워두면 무시

	SpawnOption option; // 생성할 때 액터에 추가될 옵션들

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		monster_idx = json.TryGet<MonsterIdx>("monster_idx").value_or(0);

		direction = json.TryGet<float>("direction").value_or(0.0f);
		min_count = json.TryGet<int32_t>("min_count").value_or(1);
		max_count = json.TryGet<int32_t>("max_count").value_or(1);
		respawn_term = json.TryGet<Tick>("respawn_term").value_or(0);
		//respawn_time_of_day = json.TryGet<std::string>("respawn_time_of_day").value_or("");
		return true;
	}
};

struct SpawnGroupTable : public TableStruct
{
	std::vector<SpawnEntry> entry_idxs; // 그룹에 속한 SpawnEntry

	bool is_first_leader = false; // 스폰 목록 처음이 리더

	// 그룹 레벨 쿨다운 등 추가 옵션
	Tick group_cooldown = 0;
	bool auto_activate = true;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<MonsterIdx>("idx").value_or(0);

		return true;
	}
};