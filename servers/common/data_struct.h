#pragma once

#include "pch.h"

import makga.math.vector3;

#include <vector>
#include <random>
#include <numeric>
#include <optional>
#include <algorithm>

// ���� ����ü

struct StatEffect
{
	makga::StatType type;
	float value = 0.0f;
	bool is_percentage = false;
};

struct AbilityEffect
{
	makga::AbilityType type;
	float value = 0.0f; // ȿ����
	Tick duration = 0; // ���ӽð�
};

// ������ ���̺� ����ü

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
	std::string map_name;
	std::string map_type;
	std::string zone_attribute;
	Level min_level = 0;
	Level max_level = 0;
	bool safe_zone = false;
	bool pvp_enabled = false;
	std::string bgm;
	std::string weather;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("map_id").value_or(0);
		terrain_idx = idx;
		map_name = json.TryGet<std::string>("map_name").value_or("");
		map_type = json.TryGet<std::string>("map_type").value_or("");
		zone_attribute = json.TryGet<std::string>("zone_attribute").value_or("");
		min_level = json.TryGet<Level>("min_level").value_or(0);
		max_level = json.TryGet<Level>("max_level").value_or(0);
		safe_zone = json.TryGet<bool>("safe_zone").value_or(false);
		pvp_enabled = json.TryGet<bool>("pvp_enabled").value_or(false);
		bgm = json.TryGet<std::string>("bgm").value_or("");
		weather = json.TryGet<std::string>("weather").value_or("");
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

struct MonsterTable : public TableStruct
{
	MonsterIdx monster_idx = 0;
	std::string monster_name;
	std::string monster_type;
	TableIdx map_id = 0;
	Level level = 0;
	std::string grade;
	std::string attribute;
	Hp hp = 0;
	Attack atk = 0;
	Defense def = 0;
	Exp exp = 0;
	int32_t gold = 0;

	makga::AIType ai_type = makga::AIType::AIType_NonAggressive;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("monster_id").value_or(0);
		monster_idx = idx;
		monster_name = json.TryGet<std::string>("monster_name").value_or("");
		monster_type = json.TryGet<std::string>("monster_type").value_or("");
		map_id = json.TryGet<TableIdx>("map_id").value_or(0);
		level = json.TryGet<Level>("level").value_or(0);
		grade = json.TryGet<std::string>("grade").value_or("");
		attribute = json.TryGet<std::string>("attribute").value_or("");
		hp = json.TryGet<Hp>("hp").value_or(0);
		atk = json.TryGet<Attack>("atk").value_or(0);
		def = json.TryGet<Defense>("def").value_or(0);
		exp = json.TryGet<Exp>("exp").value_or(0);
		gold = json.TryGet<int32_t>("gold").value_or(0);
		return 0 != idx;
	}
};

struct ItemRandomOptionTable : public TableStruct
{
	int32_t option_idx = 0;
	int32_t option_value = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("idx").value_or(0);
		return true;
	}
};

// ─── Spawn (flat) ──────────────────────────────────────────────────────────

struct SpawnTable : public TableStruct
{
	MonsterIdx monster_id = 0;
	TableIdx map_id = 0;
	int32_t spawn_count = 0;
	int32_t respawn_sec = 0;
	int32_t is_boss = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("spawn_id").value_or(0);
		monster_id = json.TryGet<MonsterIdx>("monster_id").value_or(0);
		map_id = json.TryGet<TableIdx>("map_id").value_or(0);
		spawn_count = json.TryGet<int32_t>("spawn_count").value_or(0);
		respawn_sec = json.TryGet<int32_t>("respawn_sec").value_or(0);
		is_boss = json.TryGet<int32_t>("is_boss").value_or(0);
		return 0 != idx;
	}
};

// ─── Achievement ───────────────────────────────────────────────────────────

struct AchievementTable : public TableStruct
{
	std::string achievement_name;
	std::string achievement_type;
	std::string condition_type;
	TableIdx condition_target_id = 0;
	int32_t condition_count = 0;
	std::string reward_type;
	TableIdx reward_id = 0;
	int32_t reward_value = 0;
	TableIdx title_id = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("achievement_id").value_or(0);
		achievement_name = json.TryGet<std::string>("achievement_name").value_or("");
		achievement_type = json.TryGet<std::string>("achievement_type").value_or("");
		condition_type = json.TryGet<std::string>("condition_type").value_or("");
		condition_target_id = json.TryGet<TableIdx>("condition_target_id").value_or(0);
		condition_count = json.TryGet<int32_t>("condition_count").value_or(0);
		reward_type = json.TryGet<std::string>("reward_type").value_or("");
		reward_id = json.TryGet<TableIdx>("reward_id").value_or(0);
		reward_value = json.TryGet<int32_t>("reward_value").value_or(0);
		title_id = json.TryGet<TableIdx>("title_id").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Character Class ───────────────────────────────────────────────────────

struct CharacterClassTable : public TableStruct
{
	std::string class_name;
	std::string description;
	Hp base_hp = 0;
	Mp base_mp = 0;
	Attack base_atk = 0;
	Attack base_matk = 0;
	Defense base_def = 0;
	Defense base_mdef = 0;
	int32_t base_spd = 0;
	std::string primary_stat;
	std::string weapon_type;
	std::string armor_type;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("class_id").value_or(0);
		class_name = json.TryGet<std::string>("class_name").value_or("");
		description = json.TryGet<std::string>("description").value_or("");
		base_hp = json.TryGet<Hp>("base_hp").value_or(0);
		base_mp = json.TryGet<Mp>("base_mp").value_or(0);
		base_atk = json.TryGet<Attack>("base_atk").value_or(0);
		base_matk = json.TryGet<Attack>("base_matk").value_or(0);
		base_def = json.TryGet<Defense>("base_def").value_or(0);
		base_mdef = json.TryGet<Defense>("base_mdef").value_or(0);
		base_spd = json.TryGet<int32_t>("base_spd").value_or(0);
		primary_stat = json.TryGet<std::string>("primary_stat").value_or("");
		weapon_type = json.TryGet<std::string>("weapon_type").value_or("");
		armor_type = json.TryGet<std::string>("armor_type").value_or("");
		return 0 != idx;
	}
};

// ─── Craft Recipe ──────────────────────────────────────────────────────────

struct CraftRecipeTable : public TableStruct
{
	TableIdx result_item_id = 0;
	int32_t result_count = 0;
	std::string craft_type;
	Level required_level = 0;
	int32_t success_rate = 100;
	int32_t exp_reward = 0;
	int32_t craft_time = 0;
	TableIdx npc_id = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("recipe_id").value_or(0);
		result_item_id = json.TryGet<TableIdx>("result_item_id").value_or(0);
		result_count = json.TryGet<int32_t>("result_count").value_or(1);
		craft_type = json.TryGet<std::string>("craft_type").value_or("");
		required_level = json.TryGet<Level>("required_level").value_or(0);
		success_rate = json.TryGet<int32_t>("success_rate").value_or(100);
		exp_reward = json.TryGet<int32_t>("exp_reward").value_or(0);
		craft_time = json.TryGet<int32_t>("craft_time").value_or(0);
		npc_id = json.TryGet<TableIdx>("npc_id").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Craft Material (1:N with recipe) ─────────────────────────────────────

struct CraftMaterialTable : public TableStruct
{
	TableIdx recipe_id = 0;
	TableIdx material_item_id = 0;
	int32_t required_count = 0;
	bool is_consumed = true;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("id").value_or(0);
		recipe_id = json.TryGet<TableIdx>("recipe_id").value_or(0);
		material_item_id = json.TryGet<TableIdx>("material_item_id").value_or(0);
		required_count = json.TryGet<int32_t>("required_count").value_or(0);
		is_consumed = json.TryGet<bool>("is_consumed").value_or(true);
		return 0 != idx;
	}
};

// ─── Currency ──────────────────────────────────────────────────────────────

struct CurrencyTable : public TableStruct
{
	std::string currency_name;
	std::string currency_type;
	std::string icon;
	int64_t max_hold = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("currency_id").value_or(0);
		currency_name = json.TryGet<std::string>("currency_name").value_or("");
		currency_type = json.TryGet<std::string>("currency_type").value_or("");
		icon = json.TryGet<std::string>("icon").value_or("");
		max_hold = json.TryGet<int64_t>("max_hold").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Dungeon ───────────────────────────────────────────────────────────────

struct DungeonTable : public TableStruct
{
	std::string dungeon_name;
	TableIdx map_id = 0;
	Level min_level = 0;
	Level max_level = 0;
	int32_t max_party = 1;
	int32_t time_limit = 0;
	int32_t daily_entry_limit = 0;
	TableIdx boss_monster_id = 0;
	TableIdx reward_item_id = 0;
	TableIdx reward_currency_id = 0;
	int32_t reward_currency_amount = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("dungeon_id").value_or(0);
		dungeon_name = json.TryGet<std::string>("dungeon_name").value_or("");
		map_id = json.TryGet<TableIdx>("map_id").value_or(0);
		min_level = json.TryGet<Level>("min_level").value_or(0);
		max_level = json.TryGet<Level>("max_level").value_or(0);
		max_party = json.TryGet<int32_t>("max_party").value_or(1);
		time_limit = json.TryGet<int32_t>("time_limit").value_or(0);
		daily_entry_limit = json.TryGet<int32_t>("daily_entry_limit").value_or(0);
		boss_monster_id = json.TryGet<TableIdx>("boss_monster_id").value_or(0);
		reward_item_id = json.TryGet<TableIdx>("reward_item_id").value_or(0);
		reward_currency_id = json.TryGet<TableIdx>("reward_currency_id").value_or(0);
		reward_currency_amount = json.TryGet<int32_t>("reward_currency_amount").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Effect ────────────────────────────────────────────────────────────────

struct EffectTable : public TableStruct
{
	std::string effect_name;
	std::string effect_type;
	std::string target;
	std::string element;
	std::string stat_type;
	std::string value_type;
	int32_t value = 0;
	Tick duration = 0;
	Tick tick_interval = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("effect_id").value_or(0);
		effect_name = json.TryGet<std::string>("effect_name").value_or("");
		effect_type = json.TryGet<std::string>("effect_type").value_or("");
		target = json.TryGet<std::string>("target").value_or("");
		element = json.TryGet<std::string>("element").value_or("");
		stat_type = json.TryGet<std::string>("stat_type").value_or("");
		value_type = json.TryGet<std::string>("value_type").value_or("");
		value = json.TryGet<int32_t>("value").value_or(0);
		duration = json.TryGet<Tick>("duration").value_or(0);
		tick_interval = json.TryGet<Tick>("tick_interval").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Gacha ─────────────────────────────────────────────────────────────────

struct GachaTable : public TableStruct
{
	std::string gacha_name;
	TableIdx cost_currency_id = 0;
	int32_t cost_amount = 0;
	int32_t pity_count = 0;
	std::string pity_guarantee_grade;
	int32_t multi_pull_count = 10;
	int32_t multi_pull_discount = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("gacha_id").value_or(0);
		gacha_name = json.TryGet<std::string>("gacha_name").value_or("");
		cost_currency_id = json.TryGet<TableIdx>("cost_currency_id").value_or(0);
		cost_amount = json.TryGet<int32_t>("cost_amount").value_or(0);
		pity_count = json.TryGet<int32_t>("pity_count").value_or(0);
		pity_guarantee_grade = json.TryGet<std::string>("pity_guarantee_grade").value_or("");
		multi_pull_count = json.TryGet<int32_t>("multi_pull_count").value_or(10);
		multi_pull_discount = json.TryGet<int32_t>("multi_pull_discount").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Gacha Item (1:N with gacha) ───────────────────────────────────────────

struct GachaItemTable : public TableStruct
{
	TableIdx gacha_id = 0;
	TableIdx item_id = 0;
	std::string item_grade;
	int32_t weight = 0;
	int32_t rate_percent = 0;
	bool is_rate_up = false;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("id").value_or(0);
		gacha_id = json.TryGet<TableIdx>("gacha_id").value_or(0);
		item_id = json.TryGet<TableIdx>("item_id").value_or(0);
		item_grade = json.TryGet<std::string>("item_grade").value_or("");
		weight = json.TryGet<int32_t>("weight").value_or(0);
		rate_percent = json.TryGet<int32_t>("rate_percent").value_or(0);
		is_rate_up = json.TryGet<bool>("is_rate_up").value_or(false);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Item Consume (1:1 with item) ──────────────────────────────────────────

struct ItemConsumeTable : public TableStruct
{
	std::string use_type;
	std::string effect_type;
	int32_t effect_value = 0;
	int32_t effect_percent = 0;
	TableIdx buff_id = 0;
	std::string target;
	std::string cooldown_group;
	int32_t cooldown_time = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("item_id").value_or(0);
		use_type = json.TryGet<std::string>("use_type").value_or("");
		effect_type = json.TryGet<std::string>("effect_type").value_or("");
		effect_value = json.TryGet<int32_t>("effect_value").value_or(0);
		effect_percent = json.TryGet<int32_t>("effect_percent").value_or(0);
		buff_id = json.TryGet<TableIdx>("buff_id").value_or(0);
		target = json.TryGet<std::string>("target").value_or("");
		cooldown_group = json.TryGet<std::string>("cooldown_group").value_or("");
		cooldown_time = json.TryGet<int32_t>("cooldown_time").value_or(0);
		return 0 != idx;
	}
};

// ─── Item Effect (1:N with item) ───────────────────────────────────────────

struct ItemEffectTable : public TableStruct
{
	TableIdx item_id = 0;
	std::string effect_type;
	TableIdx effect_id = 0;
	int32_t chance = 100;
	std::string condition;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("id").value_or(0);
		item_id = json.TryGet<TableIdx>("item_id").value_or(0);
		effect_type = json.TryGet<std::string>("effect_type").value_or("");
		effect_id = json.TryGet<TableIdx>("effect_id").value_or(0);
		chance = json.TryGet<int32_t>("chance").value_or(100);
		condition = json.TryGet<std::string>("condition").value_or("");
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Item Equip (1:1 with item) ────────────────────────────────────────────

struct ItemEquipTable : public TableStruct
{
	std::string slot;
	std::string weapon_type;
	TableIdx required_class = 0;
	Level required_level = 0;
	Attack stat_atk = 0;
	Attack stat_matk = 0;
	Defense stat_def = 0;
	Defense stat_mdef = 0;
	Hp stat_hp = 0;
	Mp stat_mp = 0;
	int32_t stat_str = 0;
	int32_t stat_int_val = 0;
	int32_t stat_dex = 0;
	int32_t stat_wis = 0;
	float critical_rate = 0.0f;
	float critical_dmg = 0.0f;
	float attack_speed = 1.0f;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("item_id").value_or(0);
		slot = json.TryGet<std::string>("slot").value_or("");
		weapon_type = json.TryGet<std::string>("weapon_type").value_or("");
		required_class = json.TryGet<TableIdx>("required_class").value_or(0);
		required_level = json.TryGet<Level>("required_level").value_or(0);
		stat_atk = json.TryGet<Attack>("stat_atk").value_or(0);
		stat_matk = json.TryGet<Attack>("stat_matk").value_or(0);
		stat_def = json.TryGet<Defense>("stat_def").value_or(0);
		stat_mdef = json.TryGet<Defense>("stat_mdef").value_or(0);
		stat_hp = json.TryGet<Hp>("stat_hp").value_or(0);
		stat_mp = json.TryGet<Mp>("stat_mp").value_or(0);
		stat_str = json.TryGet<int32_t>("stat_str").value_or(0);
		stat_int_val = json.TryGet<int32_t>("stat_int").value_or(0);
		stat_dex = json.TryGet<int32_t>("stat_dex").value_or(0);
		stat_wis = json.TryGet<int32_t>("stat_wis").value_or(0);
		critical_rate = json.TryGet<float>("critical_rate").value_or(0.0f);
		critical_dmg = json.TryGet<float>("critical_dmg").value_or(0.0f);
		attack_speed = json.TryGet<float>("attack_speed").value_or(1.0f);
		return 0 != idx;
	}
};

// ─── Item Set ──────────────────────────────────────────────────────────────

struct ItemSetTable : public TableStruct
{
	std::string set_name;
	std::string description;
	int32_t required_pieces = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("set_id").value_or(0);
		set_name = json.TryGet<std::string>("set_name").value_or("");
		description = json.TryGet<std::string>("description").value_or("");
		required_pieces = json.TryGet<int32_t>("required_pieces").value_or(0);
		return 0 != idx;
	}
};

// ─── Item Set Effect (1:N with item_set) ───────────────────────────────────

struct ItemSetEffectTable : public TableStruct
{
	TableIdx set_id = 0;
	int32_t required_pieces = 0;
	std::string effect_type;
	std::string stat_type;
	std::string value_type;
	int32_t value = 0;
	TableIdx buff_id = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("id").value_or(0);
		set_id = json.TryGet<TableIdx>("set_id").value_or(0);
		required_pieces = json.TryGet<int32_t>("required_pieces").value_or(0);
		effect_type = json.TryGet<std::string>("effect_type").value_or("");
		stat_type = json.TryGet<std::string>("stat_type").value_or("");
		value_type = json.TryGet<std::string>("value_type").value_or("");
		value = json.TryGet<int32_t>("value").value_or(0);
		buff_id = json.TryGet<TableIdx>("buff_id").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Item Upgrade (keyed by grade → level) ─────────────────────────────────

struct ItemUpgradeTable : public TableStruct
{
	std::string base_item_grade;
	int32_t upgrade_level_val = 0;
	int32_t success_rate = 100;
	int32_t destroy_rate = 0;
	int32_t atk_bonus_flat = 0;
	int32_t def_bonus_flat = 0;
	int32_t hp_bonus_flat = 0;
	TableIdx required_material_id = 0;
	int32_t required_material_count = 0;
	int32_t required_gold = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		upgrade_level_val = json.TryGet<int32_t>("upgrade_level").value_or(0);
		base_item_grade = json.TryGet<std::string>("base_item_grade").value_or("");
		idx = static_cast<TableIdx>(upgrade_level_val);
		success_rate = json.TryGet<int32_t>("success_rate").value_or(100);
		destroy_rate = json.TryGet<int32_t>("destroy_rate").value_or(0);
		atk_bonus_flat = json.TryGet<int32_t>("atk_bonus_flat").value_or(0);
		def_bonus_flat = json.TryGet<int32_t>("def_bonus_flat").value_or(0);
		hp_bonus_flat = json.TryGet<int32_t>("hp_bonus_flat").value_or(0);
		required_material_id = json.TryGet<TableIdx>("required_material_id").value_or(0);
		required_material_count = json.TryGet<int32_t>("required_material_count").value_or(0);
		required_gold = json.TryGet<int32_t>("required_gold").value_or(0);
		return !base_item_grade.empty();
	}
};

// ─── Level Exp ─────────────────────────────────────────────────────────────

struct LevelExpTable : public TableStruct
{
	Exp required_exp = 0;
	Exp cumulative_exp = 0;
	int32_t reward_gold = 0;
	int32_t reward_stat_point = 0;
	int32_t reward_skill_point = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = static_cast<TableIdx>(json.TryGet<int32_t>("level").value_or(0));
		required_exp = json.TryGet<Exp>("required_exp").value_or(0);
		cumulative_exp = json.TryGet<Exp>("cumulative_exp").value_or(0);
		reward_gold = json.TryGet<int32_t>("reward_gold").value_or(0);
		reward_stat_point = json.TryGet<int32_t>("reward_stat_point").value_or(0);
		reward_skill_point = json.TryGet<int32_t>("reward_skill_point").value_or(0);
		return 0 != idx;
	}
};

// ─── Map Object ────────────────────────────────────────────────────────────

struct MapObjectTable : public TableStruct
{
	TableIdx map_id = 0;
	std::string object_type;
	std::string object_name;
	float pos_x = 0.0f;
	float pos_y = 0.0f;
	std::string interaction_type;
	TableIdx linked_id = 0;
	int32_t respawn_time = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("object_id").value_or(0);
		map_id = json.TryGet<TableIdx>("map_id").value_or(0);
		object_type = json.TryGet<std::string>("object_type").value_or("");
		object_name = json.TryGet<std::string>("object_name").value_or("");
		pos_x = json.TryGet<float>("pos_x").value_or(0.0f);
		pos_y = json.TryGet<float>("pos_y").value_or(0.0f);
		interaction_type = json.TryGet<std::string>("interaction_type").value_or("");
		linked_id = json.TryGet<TableIdx>("linked_id").value_or(0);
		respawn_time = json.TryGet<int32_t>("respawn_time").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Monster Drop (1:N with monster) ──────────────────────────────────────

struct MonsterDropTable : public TableStruct
{
	MonsterIdx monster_id = 0;
	TableIdx item_id = 0;
	int32_t min_count = 1;
	int32_t max_count = 1;
	int32_t drop_rate = 0;
	bool is_guaranteed = false;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("id").value_or(0);
		monster_id = json.TryGet<MonsterIdx>("monster_id").value_or(0);
		item_id = json.TryGet<TableIdx>("item_id").value_or(0);
		min_count = json.TryGet<int32_t>("min_count").value_or(1);
		max_count = json.TryGet<int32_t>("max_count").value_or(1);
		drop_rate = json.TryGet<int32_t>("drop_rate").value_or(0);
		is_guaranteed = json.TryGet<bool>("is_guaranteed").value_or(false);
		return 0 != idx;
	}
};

// ─── Monster Skill (1:N with monster) ─────────────────────────────────────

struct MonsterSkillTable : public TableStruct
{
	MonsterIdx monster_id = 0;
	SkillIdx skill_id = 0;
	std::string use_condition;
	int32_t use_hp_threshold = 100;
	int32_t cooldown = 0;
	int32_t priority = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("id").value_or(0);
		monster_id = json.TryGet<MonsterIdx>("monster_id").value_or(0);
		skill_id = json.TryGet<SkillIdx>("skill_id").value_or(0);
		use_condition = json.TryGet<std::string>("use_condition").value_or("");
		use_hp_threshold = json.TryGet<int32_t>("use_hp_threshold").value_or(100);
		cooldown = json.TryGet<int32_t>("cooldown").value_or(0);
		priority = json.TryGet<int32_t>("priority").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Monster Stat (1:1 with monster) ──────────────────────────────────────

struct MonsterStatTable : public TableStruct
{
	int32_t hp_per_level = 0;
	int32_t atk_per_level = 0;
	int32_t def_per_level = 0;
	float critical_rate = 0.0f;
	int32_t critical_dmg = 150;
	int32_t skill_use_interval = 0;
	int32_t aggro_range = 0;
	int32_t flee_hp_percent = 0;
	int32_t resist_fire = 0;
	int32_t resist_ice = 0;
	int32_t resist_poison = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("monster_id").value_or(0);
		hp_per_level = json.TryGet<int32_t>("hp_per_level").value_or(0);
		atk_per_level = json.TryGet<int32_t>("atk_per_level").value_or(0);
		def_per_level = json.TryGet<int32_t>("def_per_level").value_or(0);
		critical_rate = json.TryGet<float>("critical_rate").value_or(0.0f);
		critical_dmg = json.TryGet<int32_t>("critical_dmg").value_or(150);
		skill_use_interval = json.TryGet<int32_t>("skill_use_interval").value_or(0);
		aggro_range = json.TryGet<int32_t>("aggro_range").value_or(0);
		flee_hp_percent = json.TryGet<int32_t>("flee_hp_percent").value_or(0);
		resist_fire = json.TryGet<int32_t>("resist_fire").value_or(0);
		resist_ice = json.TryGet<int32_t>("resist_ice").value_or(0);
		resist_poison = json.TryGet<int32_t>("resist_poison").value_or(0);
		return 0 != idx;
	}
};

// ─── NPC ───────────────────────────────────────────────────────────────────

struct NpcTable : public TableStruct
{
	std::string npc_name;
	std::string npc_type;
	TableIdx map_id = 0;
	float pos_x = 0.0f;
	float pos_y = 0.0f;
	std::string faction;
	bool has_shop = false;
	bool has_quest = false;
	bool has_craft = false;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("npc_id").value_or(0);
		npc_name = json.TryGet<std::string>("npc_name").value_or("");
		npc_type = json.TryGet<std::string>("npc_type").value_or("");
		map_id = json.TryGet<TableIdx>("map_id").value_or(0);
		pos_x = json.TryGet<float>("pos_x").value_or(0.0f);
		pos_y = json.TryGet<float>("pos_y").value_or(0.0f);
		faction = json.TryGet<std::string>("faction").value_or("");
		has_shop = json.TryGet<bool>("has_shop").value_or(false);
		has_quest = json.TryGet<bool>("has_quest").value_or(false);
		has_craft = json.TryGet<bool>("has_craft").value_or(false);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Quest ─────────────────────────────────────────────────────────────────

struct QuestTable : public TableStruct
{
	std::string quest_name;
	std::string quest_type;
	int32_t chain_id = 0;
	int32_t chain_order = 0;
	Level min_level = 0;
	Level max_level = 0;
	TableIdx npc_id = 0;
	TableIdx required_quest_id = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("quest_id").value_or(0);
		quest_name = json.TryGet<std::string>("quest_name").value_or("");
		quest_type = json.TryGet<std::string>("quest_type").value_or("");
		chain_id = json.TryGet<int32_t>("chain_id").value_or(0);
		chain_order = json.TryGet<int32_t>("chain_order").value_or(0);
		min_level = json.TryGet<Level>("min_level").value_or(0);
		max_level = json.TryGet<Level>("max_level").value_or(0);
		npc_id = json.TryGet<TableIdx>("npc_id").value_or(0);
		required_quest_id = json.TryGet<TableIdx>("required_quest_id").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Quest Condition (1:N with quest) ─────────────────────────────────────

struct QuestConditionTable : public TableStruct
{
	TableIdx quest_id = 0;
	std::string condition_type;
	TableIdx target_id = 0;
	int32_t target_count = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("id").value_or(0);
		quest_id = json.TryGet<TableIdx>("quest_id").value_or(0);
		condition_type = json.TryGet<std::string>("condition_type").value_or("");
		target_id = json.TryGet<TableIdx>("target_id").value_or(0);
		target_count = json.TryGet<int32_t>("target_count").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Quest Reward (1:N with quest) ────────────────────────────────────────

struct QuestRewardTable : public TableStruct
{
	TableIdx quest_id = 0;
	std::string reward_type;
	TableIdx reward_id = 0;
	int32_t reward_count = 0;
	int32_t reward_value = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("id").value_or(0);
		quest_id = json.TryGet<TableIdx>("quest_id").value_or(0);
		reward_type = json.TryGet<std::string>("reward_type").value_or("");
		reward_id = json.TryGet<TableIdx>("reward_id").value_or(0);
		reward_count = json.TryGet<int32_t>("reward_count").value_or(0);
		reward_value = json.TryGet<int32_t>("reward_value").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Ranking Reward ────────────────────────────────────────────────────────

struct RankingRewardTable : public TableStruct
{
	TableIdx season_id = 0;
	std::string ranking_type;
	int32_t rank_min = 0;
	int32_t rank_max = 0;
	std::string reward_type;
	TableIdx reward_id = 0;
	int32_t reward_count = 0;
	TableIdx reward_currency_id = 0;
	int32_t reward_currency_amount = 0;
	TableIdx title_id = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("id").value_or(0);
		season_id = json.TryGet<TableIdx>("season_id").value_or(0);
		ranking_type = json.TryGet<std::string>("ranking_type").value_or("");
		rank_min = json.TryGet<int32_t>("rank_min").value_or(0);
		rank_max = json.TryGet<int32_t>("rank_max").value_or(0);
		reward_type = json.TryGet<std::string>("reward_type").value_or("");
		reward_id = json.TryGet<TableIdx>("reward_id").value_or(0);
		reward_count = json.TryGet<int32_t>("reward_count").value_or(0);
		reward_currency_id = json.TryGet<TableIdx>("reward_currency_id").value_or(0);
		reward_currency_amount = json.TryGet<int32_t>("reward_currency_amount").value_or(0);
		title_id = json.TryGet<TableIdx>("title_id").value_or(0);
		return 0 != idx;
	}
};

// ─── Season ────────────────────────────────────────────────────────────────

struct SeasonTable : public TableStruct
{
	std::string season_name;
	std::string start_date;
	std::string end_date;
	std::string theme;
	TableIdx pass_item_id = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("season_id").value_or(0);
		season_name = json.TryGet<std::string>("season_name").value_or("");
		start_date = json.TryGet<std::string>("start_date").value_or("");
		end_date = json.TryGet<std::string>("end_date").value_or("");
		theme = json.TryGet<std::string>("theme").value_or("");
		pass_item_id = json.TryGet<TableIdx>("pass_item_id").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Shop ──────────────────────────────────────────────────────────────────

struct ShopTable : public TableStruct
{
	std::string shop_name;
	TableIdx npc_id = 0;
	std::string shop_type;
	std::string currency_type;
	int32_t restock_interval = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("shop_id").value_or(0);
		shop_name = json.TryGet<std::string>("shop_name").value_or("");
		npc_id = json.TryGet<TableIdx>("npc_id").value_or(0);
		shop_type = json.TryGet<std::string>("shop_type").value_or("");
		currency_type = json.TryGet<std::string>("currency_type").value_or("");
		restock_interval = json.TryGet<int32_t>("restock_interval").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Shop Item (1:N with shop) ─────────────────────────────────────────────

struct ShopItemTable : public TableStruct
{
	TableIdx shop_id = 0;
	TableIdx item_id = 0;
	int32_t price = 0;
	std::string currency_type;
	int32_t stock = 0;
	Level min_level = 0;
	TableIdx required_quest_id = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("id").value_or(0);
		shop_id = json.TryGet<TableIdx>("shop_id").value_or(0);
		item_id = json.TryGet<TableIdx>("item_id").value_or(0);
		price = json.TryGet<int32_t>("price").value_or(0);
		currency_type = json.TryGet<std::string>("currency_type").value_or("");
		stock = json.TryGet<int32_t>("stock").value_or(0);
		min_level = json.TryGet<Level>("min_level").value_or(0);
		required_quest_id = json.TryGet<TableIdx>("required_quest_id").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Skill Condition (1:1 with skill) ─────────────────────────────────────

struct SkillConditionTable : public TableStruct
{
	std::string condition_type;
	int32_t condition_value = 0;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("skill_id").value_or(0);
		condition_type = json.TryGet<std::string>("condition_type").value_or("");
		condition_value = json.TryGet<int32_t>("condition_value").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

// ─── Skill Effect (1:N with skill) ────────────────────────────────────────

struct SkillEffectTable : public TableStruct
{
	SkillIdx skill_id = 0;
	SkillLevel skill_level_min = 0;
	TableIdx effect_id = 0;
	std::string trigger;
	int32_t chance = 100;
	int32_t apply_order = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("id").value_or(0);
		skill_id = json.TryGet<SkillIdx>("skill_id").value_or(0);
		skill_level_min = json.TryGet<SkillLevel>("skill_level_min").value_or(0);
		effect_id = json.TryGet<TableIdx>("effect_id").value_or(0);
		trigger = json.TryGet<std::string>("trigger").value_or("");
		chance = json.TryGet<int32_t>("chance").value_or(100);
		apply_order = json.TryGet<int32_t>("apply_order").value_or(0);
		return 0 != idx;
	}
};

// ─── Stat Growth (1:1 with class) ─────────────────────────────────────────

struct StatGrowthTable : public TableStruct
{
	int32_t hp_per_level = 0;
	int32_t mp_per_level = 0;
	int32_t atk_per_level = 0;
	int32_t matk_per_level = 0;
	int32_t def_per_level = 0;
	int32_t mdef_per_level = 0;
	int32_t str_per_level = 0;
	int32_t int_per_level = 0;
	int32_t dex_per_level = 0;
	int32_t wis_per_level = 0;
	float critical_rate_per_level = 0.0f;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("class_id").value_or(0);
		hp_per_level = json.TryGet<int32_t>("hp_per_level").value_or(0);
		mp_per_level = json.TryGet<int32_t>("mp_per_level").value_or(0);
		atk_per_level = json.TryGet<int32_t>("atk_per_level").value_or(0);
		matk_per_level = json.TryGet<int32_t>("matk_per_level").value_or(0);
		def_per_level = json.TryGet<int32_t>("def_per_level").value_or(0);
		mdef_per_level = json.TryGet<int32_t>("mdef_per_level").value_or(0);
		str_per_level = json.TryGet<int32_t>("str_per_level").value_or(0);
		int_per_level = json.TryGet<int32_t>("int_per_level").value_or(0);
		dex_per_level = json.TryGet<int32_t>("dex_per_level").value_or(0);
		wis_per_level = json.TryGet<int32_t>("wis_per_level").value_or(0);
		critical_rate_per_level = json.TryGet<float>("critical_rate_per_level").value_or(0.0f);
		return 0 != idx;
	}
};

// ─── Title ─────────────────────────────────────────────────────────────────

struct TitleTable : public TableStruct
{
	std::string title_name;
	std::string title_type;
	int32_t stat_atk = 0;
	int32_t stat_def = 0;
	int32_t stat_hp = 0;
	int32_t stat_mp = 0;
	int32_t stat_spd = 0;
	float critical_rate = 0.0f;
	std::string description;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("title_id").value_or(0);
		title_name = json.TryGet<std::string>("title_name").value_or("");
		title_type = json.TryGet<std::string>("title_type").value_or("");
		stat_atk = json.TryGet<int32_t>("stat_atk").value_or(0);
		stat_def = json.TryGet<int32_t>("stat_def").value_or(0);
		stat_hp = json.TryGet<int32_t>("stat_hp").value_or(0);
		stat_mp = json.TryGet<int32_t>("stat_mp").value_or(0);
		stat_spd = json.TryGet<int32_t>("stat_spd").value_or(0);
		critical_rate = json.TryGet<float>("critical_rate").value_or(0.0f);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};
struct ItemTable : public TableStruct
{
	ItemIdx item_idx = 0;
	std::string item_name;
	std::string item_type;
	std::string item_grade;
	Level item_level = 0;
	StackCount max_stack = 1;
	float weight = 0.0f;
	int32_t sell_price = 0;
	std::string icon;
	std::string description;

	RefineLevel max_refine = 0;
	int32_t max_enchant_count = 0;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("item_id").value_or(0);
		item_idx = idx;
		item_name = json.TryGet<std::string>("item_name").value_or("");
		item_type = json.TryGet<std::string>("item_type").value_or("");
		item_grade = json.TryGet<std::string>("item_grade").value_or("");
		item_level = json.TryGet<Level>("item_level").value_or(0);
		max_stack = json.TryGet<StackCount>("max_stack").value_or(1);
		weight = json.TryGet<float>("weight").value_or(0.0f);
		sell_price = json.TryGet<int32_t>("sell_price").value_or(0);
		icon = json.TryGet<std::string>("icon").value_or("");
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

struct BuffTable : public TableStruct
{
	std::string buff_name;
	std::string buff_type;
	std::string stat_type;
	std::string value_type;
	int32_t value = 0;
	Tick duration = 0;
	std::string icon;
	bool stackable = false;
	int32_t max_stack = 1;
	std::string description;

	SkillIdx skill_idx = 0;
	Tick duration_time = 0;
	std::vector<StatEffect> stats;
	std::vector<AbilityEffect> abilities;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("buff_id").value_or(0);
		buff_name = json.TryGet<std::string>("buff_name").value_or("");
		buff_type = json.TryGet<std::string>("buff_type").value_or("");
		stat_type = json.TryGet<std::string>("stat_type").value_or("");
		value_type = json.TryGet<std::string>("value_type").value_or("");
		value = json.TryGet<int32_t>("value").value_or(0);
		duration = json.TryGet<Tick>("duration").value_or(0);
		duration_time = duration;
		icon = json.TryGet<std::string>("icon").value_or("");
		stackable = json.TryGet<bool>("stackable").value_or(false);
		max_stack = json.TryGet<int32_t>("max_stack").value_or(1);
		description = json.TryGet<std::string>("description").value_or("");
		return 0 != idx;
	}
};

struct SkillLevelTable : public TableStruct
{
	SkillIdx skill_idx = 0;
	SkillLevel level = 0;

	int32_t atk_rate = 0;
	int32_t matk_rate = 0;
	int32_t heal_rate = 0;
	int32_t effect_value = 0;
	Tick duration = 0;
	int32_t mp_cost = 0;
	float cooldown = 0.0f;

	std::vector<StatEffect> stats;
	std::vector<AbilityEffect> abilities;
	std::vector<BuffTable> buff_tables;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		skill_idx = json.TryGet<SkillIdx>("skill_id").value_or(0);
		idx = skill_idx;
		level = json.TryGet<SkillLevel>("level").value_or(0);
		atk_rate = json.TryGet<int32_t>("atk_rate").value_or(0);
		matk_rate = json.TryGet<int32_t>("matk_rate").value_or(0);
		heal_rate = json.TryGet<int32_t>("heal_rate").value_or(0);
		effect_value = json.TryGet<int32_t>("effect_value").value_or(0);
		duration = json.TryGet<Tick>("duration").value_or(0);
		mp_cost = json.TryGet<int32_t>("mp_cost").value_or(0);
		cooldown = json.TryGet<float>("cooldown").value_or(0.0f);
		return 0 != skill_idx;
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
	std::string skill_name;
	TableIdx class_id = 0;
	std::string skill_type;
	std::string target_type;
	Level unlock_level = 0;
	SkillLevel max_level = 0;
	std::string description;

	Prob success_rate = max_prob;
	makga::CostType cost_type = makga::CostType_Gold;
	int32_t cost_value = 0;
	int32_t casting_group = 0;
	Tick casting_time = 0;
	int32_t cool_group = 0;
	Tick cool_time = 0;
	int32_t hit_count = 0;
	int32_t max_duplicate_count = 0;
	int32_t max_overlap_count = 0;
	bool is_force = false;
	bool is_only_instance = false;

	std::map<SkillLevel, SkillLevelTable> level_table;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<TableIdx>("skill_id").value_or(0);
		skill_idx = idx;
		skill_name = json.TryGet<std::string>("skill_name").value_or("");
		class_id = json.TryGet<TableIdx>("class_id").value_or(0);
		skill_type = json.TryGet<std::string>("skill_type").value_or("");
		target_type = json.TryGet<std::string>("target_type").value_or("");
		unlock_level = json.TryGet<Level>("unlock_level").value_or(0);
		max_level = json.TryGet<SkillLevel>("max_level").value_or(0);
		description = json.TryGet<std::string>("description").value_or("");
		casting_time = static_cast<Tick>(json.TryGet<float>("cast_time").value_or(0.0f) * 1000);
		cool_time = static_cast<Tick>(json.TryGet<float>("cooldown").value_or(0.0f) * 1000);
		cost_value = json.TryGet<int32_t>("mp_cost").value_or(0);
		return 0 != idx;
	}
};

//////////////////////////////////////////////////////
// Spawn ���� ������ ���� (������ ���̺�)
/*
  ���� ���:
  - �� ����(SpawnPoint)�� ���� SpawnEntry �� �־� �پ��� ���͸� ������ �� ����.
  - ���� MonsterIdx �� ���� SpawnPoint �� �ݺ��� ������ ���� ���͸� ���� ��ҿ��� ����.
  - ��Ÿ�ӿ����� SpawnManager �� �Ʒ� ������ �о� ������Ʈ Ǯ���� SpawnInstance �� �Ҵ�/����.
*/
//////////////////////////////////////////////////////

struct SpawnOption
{
	Level level = 0;
};

struct SpawnEntry : public TableStruct
{
	MonsterIdx monster_idx = 0; // ������ ���� ���̺� �ε���

	makga::math::Vector3 position; // ��ġ
	float direction = 0.0f;

	int32_t min_count = 1; // ���õǾ��� �� �ּ� ���� ��
	int32_t max_count = 1; // ���õǾ��� �� �ִ� ���� ��

	Tick respawn_term = 0; // ������ �ֱ�(ƽ), 0 �̸� ��� ����
	std::string respawn_time_of_day; // Ư�� �ð� ������(HH:MM:SS), ����θ� ����

	SpawnOption option; // ������ �� ���Ϳ� �߰��� �ɼǵ�

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
	std::vector<SpawnEntry> entry_idxs; // �׷쿡 ���� SpawnEntry

	bool is_first_leader = false; // ���� ��� ó���� ����

	// �׷� ���� ��ٿ� �� �߰� �ɼ�
	Tick group_cooldown = 0;
	bool auto_activate = true;

	bool LoadFromJson(const rapidjson::Value& json) override
	{
		idx = json.TryGet<MonsterIdx>("idx").value_or(0);

		return true;
	}
};