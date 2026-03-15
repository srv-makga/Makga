#pragma once

#include "guild_member.h"
#include "guild_inventory.h"

import makga.lib.lock;

// ─── GuildCraft ────────────────────────────────────────────────
struct CraftSlot
{
	UserUid contributor = 0;
	TableIdx item_idx = 0;
	StackCount count = 0;
};

struct GuildCraftRecipe
{
	uint32_t recipe_id = 0;
	TableIdx result_idx = 0;
	StackCount result_count = 1;
	std::vector<CraftSlot> required;
};

// @brief 길드원이 재료를 기탁하고 제작 완성 시 GuildInventory로 결과물이 추가되는 제작 시스템.
class GuildCraft
{
public:
	// @brief 재료 기탁 — GameServer가 유저 인벤토리에서 차감 후 이 메서드 호출
	bool RegisterMaterial(UserUid contributor, TableIdx item_idx, StackCount count);

	// @brief 기탁된 재료로 레시피 제작 가능 여부
	bool CanCraft(uint32_t recipe_id) const;

	// @brief 제작 실행 (Master/SubMaster 전용) — 성공 시 GuildInventory에 결과물 추가
	bool StartCraft(const GuildMember& requester, uint32_t recipe_id, GuildInventory& inventory,
		std::shared_ptr<ItemObject> result_item);

	virtual void OnCraftComplete(const GuildCraftRecipe& recipe) {}

	void LoadRecipes(std::vector<GuildCraftRecipe> recipes);
	const std::vector<CraftSlot>& GetDepositedMaterials() const;

private:
	const GuildCraftRecipe* FindRecipe(uint32_t recipe_id) const;
	bool HasMaterials(const GuildCraftRecipe& recipe) const;
	void ConsumeMaterials(const GuildCraftRecipe& recipe);

	std::vector<CraftSlot> deposited_;
	std::vector<GuildCraftRecipe> recipes_;
	mutable makga::lib::Mutex craft_mutex_;
};
