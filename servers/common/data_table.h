#pragma once

#include "data_struct.h"

import makga.lib.pattern.singleton;

template<typename Key, typename Value>
class Table
{
public:
	using Container = std::map<Key, Value>;
	using Iterator = typename Container::iterator;
	using ConstIterator = typename Container::const_iterator;

	Table() = default;
	~Table() = default;

	std::optional<const Value*> TryGet(Key key) const
	{
		auto it = data_.find(key);
		if (it == data_.end()) return std::nullopt;
		return &it->second;
	}

	Value& Get(Key key)
	{
		return data_[key];
	}

	std::pair<Iterator, bool> Emplace(Key key, Value value)
	{
		return data_.emplace(key, std::move(value));
	}

	void Insert(Key key, Value value)
	{
		data_.insert_or_assign(key, std::move(value));
	}

	bool Contains(Key key) const
	{
		return data_.find(key) != data_.end();
	}

	bool Erase(Key key)
	{
		return 0 < data_.erase(key);
	}

	Iterator begin() { return data_.begin(); }
	Iterator end() { return data_.end(); }
	ConstIterator begin() const { return data_.begin(); }
	ConstIterator end() const { return data_.end(); }
	ConstIterator cbegin() const { return data_.cbegin(); }
	ConstIterator cend() const { return data_.cend(); }

	void Clear()
	{
		data_.clear();
	}

	std::size_t Size() const
	{
		return data_.size();
	}

	bool Empty() const
	{
		return data_.empty();
	}

private:
	Container data_;
};

class DataTable : public makga::lib::Singleton<DataTable>
{
public:
	// ─── Simple tables (keyed by primary ID) ───────────────────────────────
	using MapTableTable          = Table<TableIdx, MapTable>;
	using MonsterTableTable      = Table<TableIdx, MonsterTable>;
	using SkillTableTable        = Table<TableIdx, SkillTable>;
	using SpawnGroupTableTable   = Table<TableIdx, SpawnGroupTable>;
	using SpawnTableTable        = Table<TableIdx, SpawnTable>;
	using AchievementTableTable  = Table<TableIdx, AchievementTable>;
	using BuffTableTable         = Table<TableIdx, BuffTable>;
	using CharacterClassTableTable = Table<TableIdx, CharacterClassTable>;
	using CraftRecipeTableTable  = Table<TableIdx, CraftRecipeTable>;
	using CurrencyTableTable     = Table<TableIdx, CurrencyTable>;
	using DungeonTableTable      = Table<TableIdx, DungeonTable>;
	using EffectTableTable       = Table<TableIdx, EffectTable>;
	using GachaTableTable        = Table<TableIdx, GachaTable>;
	using ItemTableTable         = Table<TableIdx, ItemTable>;
	using ItemConsumeTableTable  = Table<TableIdx, ItemConsumeTable>;
	using ItemEquipTableTable    = Table<TableIdx, ItemEquipTable>;
	using ItemSetTableTable      = Table<TableIdx, ItemSetTable>;
	using LevelExpTableTable     = Table<TableIdx, LevelExpTable>;
	using MapObjectTableTable    = Table<TableIdx, MapObjectTable>;
	using MonsterStatTableTable  = Table<TableIdx, MonsterStatTable>;
	using NpcTableTable          = Table<TableIdx, NpcTable>;
	using QuestTableTable        = Table<TableIdx, QuestTable>;
	using RankingRewardTableTable = Table<TableIdx, RankingRewardTable>;
	using SeasonTableTable       = Table<TableIdx, SeasonTable>;
	using ShopTableTable         = Table<TableIdx, ShopTable>;
	using SkillConditionTableTable = Table<TableIdx, SkillConditionTable>;
	using StatGrowthTableTable   = Table<TableIdx, StatGrowthTable>;
	using TitleTableTable        = Table<TableIdx, TitleTable>;

	// ─── 1:N tables (keyed by foreign/parent ID → vector) ─────────────────
	using CraftMaterialMap   = std::unordered_map<TableIdx, std::vector<CraftMaterialTable>>;
	using GachaItemMap       = std::unordered_map<TableIdx, std::vector<GachaItemTable>>;
	using ItemEffectMap      = std::unordered_map<TableIdx, std::vector<ItemEffectTable>>;
	using ItemSetEffectMap   = std::unordered_map<TableIdx, std::vector<ItemSetEffectTable>>;
	using MonsterDropMap     = std::unordered_map<TableIdx, std::vector<MonsterDropTable>>;
	using MonsterSkillMap    = std::unordered_map<TableIdx, std::vector<MonsterSkillTable>>;
	using QuestConditionMap  = std::unordered_map<TableIdx, std::vector<QuestConditionTable>>;
	using QuestRewardMap     = std::unordered_map<TableIdx, std::vector<QuestRewardTable>>;
	using ShopItemMap        = std::unordered_map<TableIdx, std::vector<ShopItemTable>>;
	using SkillEffectMap     = std::unordered_map<TableIdx, std::vector<SkillEffectTable>>;

	// ─── Item upgrade: keyed by grade → (level → data) ────────────────────
	using ItemUpgradeMap = std::unordered_map<std::string, std::map<int32_t, ItemUpgradeTable>>;

	DataTable() = default;
	virtual ~DataTable() = default;

	virtual bool LoadTable(const std::string& base_path = "");

public:
	bool DataVerification();
	bool LoadFileToBuffer(const std::string& file_path, rapidjson::Document& doc);

	// ─── Simple table loaders ──────────────────────────────────────────────
	bool LoadMapTable(const std::string& file_path);
	bool LoadMonsterTable(const std::string& file_path);
	bool LoadSkillTable(const std::string& file_path);
	bool LoadSpawnGroupTable(const std::string& file_path);
	bool LoadSpawnTable(const std::string& file_path);
	bool LoadAchievementTable(const std::string& file_path);
	bool LoadBuffTable(const std::string& file_path);
	bool LoadCharacterClassTable(const std::string& file_path);
	bool LoadCraftRecipeTable(const std::string& file_path);
	bool LoadCurrencyTable(const std::string& file_path);
	bool LoadDungeonTable(const std::string& file_path);
	bool LoadEffectTable(const std::string& file_path);
	bool LoadGachaTable(const std::string& file_path);
	bool LoadItemTable(const std::string& file_path);
	bool LoadItemConsumeTable(const std::string& file_path);
	bool LoadItemEquipTable(const std::string& file_path);
	bool LoadItemSetTable(const std::string& file_path);
	bool LoadLevelExpTable(const std::string& file_path);
	bool LoadMapObjectTable(const std::string& file_path);
	bool LoadMonsterStatTable(const std::string& file_path);
	bool LoadNpcTable(const std::string& file_path);
	bool LoadQuestTable(const std::string& file_path);
	bool LoadRankingRewardTable(const std::string& file_path);
	bool LoadSeasonTable(const std::string& file_path);
	bool LoadShopTable(const std::string& file_path);
	bool LoadSkillConditionTable(const std::string& file_path);
	bool LoadStatGrowthTable(const std::string& file_path);
	bool LoadTitleTable(const std::string& file_path);

	// ─── 1:N table loaders ────────────────────────────────────────────────
	bool LoadCraftMaterialTable(const std::string& file_path);
	bool LoadGachaItemTable(const std::string& file_path);
	bool LoadItemEffectTable(const std::string& file_path);
	bool LoadItemSetEffectTable(const std::string& file_path);
	bool LoadItemUpgradeTable(const std::string& file_path);
	bool LoadMonsterDropTable(const std::string& file_path);
	bool LoadMonsterSkillTable(const std::string& file_path);
	bool LoadQuestConditionTable(const std::string& file_path);
	bool LoadQuestRewardTable(const std::string& file_path);
	bool LoadShopItemTable(const std::string& file_path);
	bool LoadSkillEffectTable(const std::string& file_path);
	bool LoadSkillLevelTable(const std::string& file_path);

	// ─── TryGet accessors ─────────────────────────────────────────────────
	std::optional<const SpawnGroupTable*> TryGetSpawnGroupTable(TableIdx idx) const
	{
		return spawn_group_table_.TryGet(idx);
	}
	std::optional<const MapTable*> TryGetMapTable(TableIdx idx) const { return map_table_.TryGet(idx); }
	std::optional<const MonsterTable*> TryGetMonsterTable(TableIdx idx) const { return monster_table_.TryGet(idx); }
	std::optional<const SkillTable*> TryGetSkillTable(TableIdx idx) const { return skill_table_.TryGet(idx); }
	std::optional<const SpawnTable*> TryGetSpawnTable(TableIdx idx) const { return spawn_table_.TryGet(idx); }
	std::optional<const AchievementTable*> TryGetAchievementTable(TableIdx idx) const { return achievement_table_.TryGet(idx); }
	std::optional<const BuffTable*> TryGetBuffTable(TableIdx idx) const { return buff_table_.TryGet(idx); }
	std::optional<const CharacterClassTable*> TryGetCharacterClassTable(TableIdx idx) const { return character_class_table_.TryGet(idx); }
	std::optional<const CraftRecipeTable*> TryGetCraftRecipeTable(TableIdx idx) const { return craft_recipe_table_.TryGet(idx); }
	std::optional<const CurrencyTable*> TryGetCurrencyTable(TableIdx idx) const { return currency_table_.TryGet(idx); }
	std::optional<const DungeonTable*> TryGetDungeonTable(TableIdx idx) const { return dungeon_table_.TryGet(idx); }
	std::optional<const EffectTable*> TryGetEffectTable(TableIdx idx) const { return effect_table_.TryGet(idx); }
	std::optional<const GachaTable*> TryGetGachaTable(TableIdx idx) const { return gacha_table_.TryGet(idx); }
	std::optional<const ItemTable*> TryGetItemTable(TableIdx idx) const { return item_table_.TryGet(idx); }
	std::optional<const ItemConsumeTable*> TryGetItemConsumeTable(TableIdx idx) const { return item_consume_table_.TryGet(idx); }
	std::optional<const ItemEquipTable*> TryGetItemEquipTable(TableIdx idx) const { return item_equip_table_.TryGet(idx); }
	std::optional<const ItemSetTable*> TryGetItemSetTable(TableIdx idx) const { return item_set_table_.TryGet(idx); }
	std::optional<const LevelExpTable*> TryGetLevelExpTable(TableIdx level) const { return level_exp_table_.TryGet(level); }
	std::optional<const MapObjectTable*> TryGetMapObjectTable(TableIdx idx) const { return map_object_table_.TryGet(idx); }
	std::optional<const MonsterStatTable*> TryGetMonsterStatTable(TableIdx monster_id) const { return monster_stat_table_.TryGet(monster_id); }
	std::optional<const NpcTable*> TryGetNpcTable(TableIdx idx) const { return npc_table_.TryGet(idx); }
	std::optional<const QuestTable*> TryGetQuestTable(TableIdx idx) const { return quest_table_.TryGet(idx); }
	std::optional<const RankingRewardTable*> TryGetRankingRewardTable(TableIdx idx) const { return ranking_reward_table_.TryGet(idx); }
	std::optional<const SeasonTable*> TryGetSeasonTable(TableIdx idx) const { return season_table_.TryGet(idx); }
	std::optional<const ShopTable*> TryGetShopTable(TableIdx idx) const { return shop_table_.TryGet(idx); }
	std::optional<const SkillConditionTable*> TryGetSkillConditionTable(TableIdx skill_id) const { return skill_condition_table_.TryGet(skill_id); }
	std::optional<const StatGrowthTable*> TryGetStatGrowthTable(TableIdx class_id) const { return stat_growth_table_.TryGet(class_id); }
	std::optional<const TitleTable*> TryGetTitleTable(TableIdx idx) const { return title_table_.TryGet(idx); }

	// ─── 1:N TryGet accessors ─────────────────────────────────────────────
	const std::vector<CraftMaterialTable>* TryGetCraftMaterials(TableIdx recipe_id) const
	{
		auto it = craft_material_map_.find(recipe_id);
		return it != craft_material_map_.end() ? &it->second : nullptr;
	}
	const std::vector<GachaItemTable>* TryGetGachaItems(TableIdx gacha_id) const
	{
		auto it = gacha_item_map_.find(gacha_id);
		return it != gacha_item_map_.end() ? &it->second : nullptr;
	}
	const std::vector<ItemEffectTable>* TryGetItemEffects(TableIdx item_id) const
	{
		auto it = item_effect_map_.find(item_id);
		return it != item_effect_map_.end() ? &it->second : nullptr;
	}
	const std::vector<ItemSetEffectTable>* TryGetItemSetEffects(TableIdx set_id) const
	{
		auto it = item_set_effect_map_.find(set_id);
		return it != item_set_effect_map_.end() ? &it->second : nullptr;
	}
	const std::vector<MonsterDropTable>* TryGetMonsterDrops(TableIdx monster_id) const
	{
		auto it = monster_drop_map_.find(monster_id);
		return it != monster_drop_map_.end() ? &it->second : nullptr;
	}
	const std::vector<MonsterSkillTable>* TryGetMonsterSkills(TableIdx monster_id) const
	{
		auto it = monster_skill_map_.find(monster_id);
		return it != monster_skill_map_.end() ? &it->second : nullptr;
	}
	const std::vector<QuestConditionTable>* TryGetQuestConditions(TableIdx quest_id) const
	{
		auto it = quest_condition_map_.find(quest_id);
		return it != quest_condition_map_.end() ? &it->second : nullptr;
	}
	const std::vector<QuestRewardTable>* TryGetQuestRewards(TableIdx quest_id) const
	{
		auto it = quest_reward_map_.find(quest_id);
		return it != quest_reward_map_.end() ? &it->second : nullptr;
	}
	const std::vector<ShopItemTable>* TryGetShopItems(TableIdx shop_id) const
	{
		auto it = shop_item_map_.find(shop_id);
		return it != shop_item_map_.end() ? &it->second : nullptr;
	}
	const std::vector<SkillEffectTable>* TryGetSkillEffects(TableIdx skill_id) const
	{
		auto it = skill_effect_map_.find(skill_id);
		return it != skill_effect_map_.end() ? &it->second : nullptr;
	}
	std::optional<const ItemUpgradeTable*> TryGetItemUpgrade(const std::string& grade, int32_t upgrade_level) const
	{
		auto grade_it = item_upgrade_map_.find(grade);
		if (grade_it == item_upgrade_map_.end()) return std::nullopt;
		auto level_it = grade_it->second.find(upgrade_level);
		if (level_it == grade_it->second.end()) return std::nullopt;
		return &level_it->second;
	}

protected:
	std::vector<char> file_buffer_;

	// ─── Simple table storage ──────────────────────────────────────────────
	MapTableTable           map_table_;
	MonsterTableTable       monster_table_;
	SkillTableTable         skill_table_;
	SpawnGroupTableTable    spawn_group_table_;
	SpawnTableTable         spawn_table_;
	AchievementTableTable   achievement_table_;
	BuffTableTable          buff_table_;
	CharacterClassTableTable character_class_table_;
	CraftRecipeTableTable   craft_recipe_table_;
	CurrencyTableTable      currency_table_;
	DungeonTableTable       dungeon_table_;
	EffectTableTable        effect_table_;
	GachaTableTable         gacha_table_;
	ItemTableTable          item_table_;
	ItemConsumeTableTable   item_consume_table_;
	ItemEquipTableTable     item_equip_table_;
	ItemSetTableTable       item_set_table_;
	LevelExpTableTable      level_exp_table_;
	MapObjectTableTable     map_object_table_;
	MonsterStatTableTable   monster_stat_table_;
	NpcTableTable           npc_table_;
	QuestTableTable         quest_table_;
	RankingRewardTableTable ranking_reward_table_;
	SeasonTableTable        season_table_;
	ShopTableTable          shop_table_;
	SkillConditionTableTable skill_condition_table_;
	StatGrowthTableTable    stat_growth_table_;
	TitleTableTable         title_table_;

	// ─── 1:N table storage ────────────────────────────────────────────────
	CraftMaterialMap    craft_material_map_;
	GachaItemMap        gacha_item_map_;
	ItemEffectMap       item_effect_map_;
	ItemSetEffectMap    item_set_effect_map_;
	MonsterDropMap      monster_drop_map_;
	MonsterSkillMap     monster_skill_map_;
	QuestConditionMap   quest_condition_map_;
	QuestRewardMap      quest_reward_map_;
	ShopItemMap         shop_item_map_;
	SkillEffectMap      skill_effect_map_;
	ItemUpgradeMap      item_upgrade_map_;
};