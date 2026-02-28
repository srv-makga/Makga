#include "pch.h"
#include "data_table.h"
#include "../../3rdparty/rapidjson/include/rapidjson/document.h"

import makga.lib.logger;

bool DataTable::LoadTable(const std::string& base_path)
{
    using Loader = bool (DataTable::*)(const std::string&);

    const std::pair<Loader, const char*> tasks[] = {
        { &DataTable::LoadMapTable,           "map.json" },
        { &DataTable::LoadMonsterTable,       "monster.json" },
        { &DataTable::LoadSkillTable,         "skill.json" },
        { &DataTable::LoadSpawnTable,         "spawn.json" },
        { &DataTable::LoadAchievementTable,   "achievement.json" },
        { &DataTable::LoadBuffTable,          "buff.json" },
        { &DataTable::LoadCharacterClassTable,"character_class.json" },
        { &DataTable::LoadCraftRecipeTable,   "craft_recipe.json" },
        { &DataTable::LoadCraftMaterialTable, "craft_material.json" },
        { &DataTable::LoadCurrencyTable,      "currency.json" },
        { &DataTable::LoadDungeonTable,       "dungeon.json" },
        { &DataTable::LoadEffectTable,        "effect.json" },
        { &DataTable::LoadGachaTable,         "gacha.json" },
        { &DataTable::LoadGachaItemTable,     "gacha_item.json" },
        { &DataTable::LoadItemTable,          "item.json" },
        { &DataTable::LoadItemConsumeTable,   "item_consume.json" },
        { &DataTable::LoadItemEffectTable,    "item_effect.json" },
        { &DataTable::LoadItemEquipTable,     "item_equip.json" },
        { &DataTable::LoadItemSetTable,       "item_set.json" },
        { &DataTable::LoadItemSetEffectTable, "item_set_effect.json" },
        { &DataTable::LoadItemUpgradeTable,   "item_upgrade.json" },
        { &DataTable::LoadLevelExpTable,      "level_exp.json" },
        { &DataTable::LoadMapObjectTable,     "map_object.json" },
        { &DataTable::LoadMonsterDropTable,   "monster_drop.json" },
        { &DataTable::LoadMonsterSkillTable,  "monster_skill.json" },
        { &DataTable::LoadMonsterStatTable,   "monster_stat.json" },
        { &DataTable::LoadNpcTable,           "npc.json" },
        { &DataTable::LoadQuestTable,         "quest.json" },
        { &DataTable::LoadQuestConditionTable,"quest_condition.json" },
        { &DataTable::LoadQuestRewardTable,   "quest_reward.json" },
        { &DataTable::LoadRankingRewardTable, "ranking_reward.json" },
        { &DataTable::LoadSeasonTable,        "season.json" },
        { &DataTable::LoadShopTable,          "shop.json" },
        { &DataTable::LoadShopItemTable,      "shop_item.json" },
        { &DataTable::LoadSkillConditionTable,"skill_condition.json" },
        { &DataTable::LoadSkillEffectTable,   "skill_effect.json" },
        { &DataTable::LoadSkillLevelTable,    "skill_level.json" },
        { &DataTable::LoadStatGrowthTable,    "stat_growth.json" },
        { &DataTable::LoadTitleTable,         "title.json" },
    };

    for (auto const& t : tasks)
	{
		if (!(this->*(t.first))(base_path + t.second))
		{
			return false;
		}
    }

	return DataVerification();
}

bool DataTable::DataVerification()
{
	return true;
}
 
bool DataTable::LoadFileToBuffer(const std::string& file_path, rapidjson::Document& doc)
{
	std::ifstream ifs(file_path, std::ios::binary | std::ios::ate);
	if (!ifs)
	{
		return false;
	}

	std::streamsize sz = ifs.tellg();
	if (0 > sz)
	{
		return false;
	}
	
	ifs.seekg(0, std::ios::beg);

	file_buffer_.resize(static_cast<size_t>(sz) + 1);

	if (!ifs.read(file_buffer_.data(), sz))
	{
		return false;
	}

	file_buffer_[static_cast<size_t>(sz)] = '\0'; // RapidJSON insitu�� �� ����

	// ParseInsitu�� ���۸� �����ϹǷ�, ���� ������ �ʿ��ϸ� �����ϰų� Parse ���
	rapidjson::ParseResult pr = doc.ParseInsitu(file_buffer_.data());
	return !pr.IsError();
}

bool DataTable::LoadMapTable(const std::string& file_path)
{
	rapidjson::Document doc;
	if (false == LoadFileToBuffer(file_path, doc))
	{
		makga::lib::MakgaLogger::Error() << "Failed to load file:" << file_path;
		return false;
	}

	auto array = doc.GetArray();
	for (auto& v : array)
	{
		if (false == v.IsObject()) continue;
		MapTable t{};
		if (false == t.LoadFromJson(v))
		{
			makga::lib::MakgaLogger::Error() << "Failed to parse map table entry:" << file_path;
			return false;
		}
		map_table_.Insert(t.idx, std::move(t));
	}

	if (true == map_table_.Empty())
	{
		makga::lib::MakgaLogger::Error() << "No map table data found:" << file_path;
		return false;
	}

	return true;
}

bool DataTable::LoadMonsterTable(const std::string& file_path)
{
	rapidjson::Document doc;
	if (false == LoadFileToBuffer(file_path, doc))
	{
		makga::lib::MakgaLogger::Error() << "Failed to load file:" << file_path;
		return false;
	}

	auto array = doc.GetArray();
	for (auto& v : array)
	{
		if (false == v.IsObject()) continue;
		MonsterTable t{};
		if (false == t.LoadFromJson(v))
		{
			makga::lib::MakgaLogger::Error() << "Failed to parse monster table entry:" << file_path;
			return false;
		}
		monster_table_.Insert(t.idx, std::move(t));
	}

	if (true == monster_table_.Empty())
	{
		makga::lib::MakgaLogger::Error() << "No monster table data found:" << file_path;
		return false;
	}

	return true;
}

bool DataTable::LoadSkillTable(const std::string& file_path)
{
	rapidjson::Document doc;
	if (false == LoadFileToBuffer(file_path, doc))
	{
		makga::lib::MakgaLogger::Error() << "Failed to load file:" << file_path;
		return false;
	}

	auto array = doc.GetArray();
	for (auto& v : array)
	{
		if (false == v.IsObject()) continue;
		SkillTable t{};
		if (false == t.LoadFromJson(v))
		{
			makga::lib::MakgaLogger::Error() << "Failed to parse skill table entry:" << file_path;
			return false;
		}
		skill_table_.Insert(t.idx, std::move(t));
	}

	if (true == skill_table_.Empty())
	{
		makga::lib::MakgaLogger::Error() << "No skill table data found:" << file_path;
		return false;
	}

	return true;
}

bool DataTable::LoadSpawnGroupTable(const std::string& file_path)
{
	rapidjson::Document doc;
	if (false == LoadFileToBuffer(file_path, doc))
	{
		makga::lib::MakgaLogger::Error() << "Failed to load file:" << file_path;
		return false;
	}

	auto array = doc.GetArray();
	for (auto& v : array)
	{
		if (false == v.IsObject())
		{
			continue;
		}

		SpawnGroupTable spawn_group_table{};
		if (false == spawn_group_table.LoadFromJson(v))
		{
			makga::lib::MakgaLogger::Error() << "Failed to load spawn group table data:" << file_path;
			return false;
		}

		spawn_group_table_.Insert(spawn_group_table.idx, std::move(spawn_group_table));
	}

	if (true == spawn_group_table_.Empty())
	{
		makga::lib::MakgaLogger::Error() << "No spawn group table data found:" << file_path;
		return false;
	}

	return true;
}

// ─── Simple table loaders ────────────────────────────────────────────────────

#define DEFINE_SIMPLE_LOADER(FnName, TableType, MemberTable) \
bool DataTable::FnName(const std::string& file_path) \
{ \
	rapidjson::Document doc; \
	if (false == LoadFileToBuffer(file_path, doc)) \
	{ \
		makga::lib::MakgaLogger::Error() << "Failed to load file:" << file_path; \
		return false; \
	} \
	auto array = doc.GetArray(); \
	for (auto& v : array) \
	{ \
		if (false == v.IsObject()) continue; \
		TableType t{}; \
		if (false == t.LoadFromJson(v)) \
		{ \
			makga::lib::MakgaLogger::Error() << "Failed to parse " #TableType " entry:" << file_path; \
			return false; \
		} \
		MemberTable.Insert(t.idx, std::move(t)); \
	} \
	if (true == MemberTable.Empty()) \
	{ \
		makga::lib::MakgaLogger::Error() << "No " #TableType " data found:" << file_path; \
		return false; \
	} \
	return true; \
}

DEFINE_SIMPLE_LOADER(LoadSpawnTable,          SpawnTable,          spawn_table_)
DEFINE_SIMPLE_LOADER(LoadAchievementTable,    AchievementTable,    achievement_table_)
DEFINE_SIMPLE_LOADER(LoadBuffTable,           BuffTable,           buff_table_)
DEFINE_SIMPLE_LOADER(LoadCharacterClassTable, CharacterClassTable, character_class_table_)
DEFINE_SIMPLE_LOADER(LoadCraftRecipeTable,    CraftRecipeTable,    craft_recipe_table_)
DEFINE_SIMPLE_LOADER(LoadCurrencyTable,       CurrencyTable,       currency_table_)
DEFINE_SIMPLE_LOADER(LoadDungeonTable,        DungeonTable,        dungeon_table_)
DEFINE_SIMPLE_LOADER(LoadEffectTable,         EffectTable,         effect_table_)
DEFINE_SIMPLE_LOADER(LoadGachaTable,          GachaTable,          gacha_table_)
DEFINE_SIMPLE_LOADER(LoadItemTable,           ItemTable,           item_table_)
DEFINE_SIMPLE_LOADER(LoadItemConsumeTable,    ItemConsumeTable,    item_consume_table_)
DEFINE_SIMPLE_LOADER(LoadItemEquipTable,      ItemEquipTable,      item_equip_table_)
DEFINE_SIMPLE_LOADER(LoadItemSetTable,        ItemSetTable,        item_set_table_)
DEFINE_SIMPLE_LOADER(LoadLevelExpTable,       LevelExpTable,       level_exp_table_)
DEFINE_SIMPLE_LOADER(LoadMapObjectTable,      MapObjectTable,      map_object_table_)
DEFINE_SIMPLE_LOADER(LoadMonsterStatTable,    MonsterStatTable,    monster_stat_table_)
DEFINE_SIMPLE_LOADER(LoadNpcTable,            NpcTable,            npc_table_)
DEFINE_SIMPLE_LOADER(LoadQuestTable,          QuestTable,          quest_table_)
DEFINE_SIMPLE_LOADER(LoadRankingRewardTable,  RankingRewardTable,  ranking_reward_table_)
DEFINE_SIMPLE_LOADER(LoadSeasonTable,         SeasonTable,         season_table_)
DEFINE_SIMPLE_LOADER(LoadShopTable,           ShopTable,           shop_table_)
DEFINE_SIMPLE_LOADER(LoadSkillConditionTable, SkillConditionTable, skill_condition_table_)
DEFINE_SIMPLE_LOADER(LoadStatGrowthTable,     StatGrowthTable,     stat_growth_table_)
DEFINE_SIMPLE_LOADER(LoadTitleTable,          TitleTable,          title_table_)

#undef DEFINE_SIMPLE_LOADER

// ─── 1:N table loaders ──────────────────────────────────────────────────────

#define DEFINE_MULTI_LOADER(FnName, TableType, MapMember, FkField) \
bool DataTable::FnName(const std::string& file_path) \
{ \
	rapidjson::Document doc; \
	if (false == LoadFileToBuffer(file_path, doc)) \
	{ \
		makga::lib::MakgaLogger::Error() << "Failed to load file:" << file_path; \
		return false; \
	} \
	auto array = doc.GetArray(); \
	for (auto& v : array) \
	{ \
		if (false == v.IsObject()) continue; \
		TableType t{}; \
		if (false == t.LoadFromJson(v)) \
		{ \
			makga::lib::MakgaLogger::Error() << "Failed to parse " #TableType " entry:" << file_path; \
			return false; \
		} \
		MapMember[t.FkField].push_back(std::move(t)); \
	} \
	return true; \
}

DEFINE_MULTI_LOADER(LoadCraftMaterialTable,  CraftMaterialTable,  craft_material_map_,  recipe_id)
DEFINE_MULTI_LOADER(LoadGachaItemTable,      GachaItemTable,      gacha_item_map_,      gacha_id)
DEFINE_MULTI_LOADER(LoadItemEffectTable,     ItemEffectTable,     item_effect_map_,     item_id)
DEFINE_MULTI_LOADER(LoadItemSetEffectTable,  ItemSetEffectTable,  item_set_effect_map_, set_id)
DEFINE_MULTI_LOADER(LoadMonsterDropTable,    MonsterDropTable,    monster_drop_map_,    monster_id)
DEFINE_MULTI_LOADER(LoadMonsterSkillTable,   MonsterSkillTable,   monster_skill_map_,   monster_id)
DEFINE_MULTI_LOADER(LoadQuestConditionTable, QuestConditionTable, quest_condition_map_, quest_id)
DEFINE_MULTI_LOADER(LoadQuestRewardTable,    QuestRewardTable,    quest_reward_map_,    quest_id)
DEFINE_MULTI_LOADER(LoadShopItemTable,       ShopItemTable,       shop_item_map_,       shop_id)
DEFINE_MULTI_LOADER(LoadSkillEffectTable,    SkillEffectTable,    skill_effect_map_,    skill_id)

#undef DEFINE_MULTI_LOADER

// ─── Special loaders ─────────────────────────────────────────────────────────

bool DataTable::LoadSkillLevelTable(const std::string& file_path)
{
	rapidjson::Document doc;
	if (false == LoadFileToBuffer(file_path, doc))
	{
		makga::lib::MakgaLogger::Error() << "Failed to load file:" << file_path;
		return false;
	}

	auto array = doc.GetArray();
	for (auto& v : array)
	{
		if (false == v.IsObject()) continue;
		SkillLevelTable t{};
		if (false == t.LoadFromJson(v))
		{
			makga::lib::MakgaLogger::Error() << "Failed to parse SkillLevelTable entry:" << file_path;
			return false;
		}
		auto opt = skill_table_.TryGet(t.idx);
		if (!opt.has_value())
		{
			makga::lib::MakgaLogger::Error() << "SkillLevelTable references unknown skill_id in:" << file_path;
			return false;
		}
		const_cast<SkillTable*>(*opt)->level_table[t.level] = std::move(t);
	}

	return true;
}

bool DataTable::LoadItemUpgradeTable(const std::string& file_path)
{
	rapidjson::Document doc;
	if (false == LoadFileToBuffer(file_path, doc))
	{
		makga::lib::MakgaLogger::Error() << "Failed to load file:" << file_path;
		return false;
	}

	auto array = doc.GetArray();
	for (auto& v : array)
	{
		if (false == v.IsObject()) continue;
		ItemUpgradeTable t{};
		if (false == t.LoadFromJson(v))
		{
			makga::lib::MakgaLogger::Error() << "Failed to parse ItemUpgradeTable entry:" << file_path;
			return false;
		}
		item_upgrade_map_[t.base_item_grade][t.upgrade_level_val] = std::move(t);
	}

	return true;
}
