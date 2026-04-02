#pragma once

#include "guild_member.h"
#include "guild_inventory.h"

// ─── GuildShop ─────────────────────────────────────────────────
enum class ShopItemType { Buff, Item };

struct GuildShopItem
{
	uint32_t shop_id = 0;
	ShopItemType type = ShopItemType::Item;
	TableIdx ref_idx = 0; // 버프 테이블 or 아이템 테이블 인덱스
	StackCount cost = 0; // 소모 재화 (ItemIdx=0 기준 인벤 내 재화 아이템)
	TableIdx currency_item_idx = 0; // 소모할 재화 아이템 인덱스
	std::string name;
};

// @brief 길드 인벤토리의 재화로 버프/아이템을 구매하는 길드 상점.
class GuildShop
{
public:
	// @brief 버프 구매 — 재화 차감 후 모든 길드원에게 버프 적용 요청 (GameServer 위임)
	bool BuyBuff(const GuildMember& requester, uint32_t shop_id, GuildInventory& inventory);

	// @brief 아이템 구매 — 재화 차감 후 GuildInventory에 아이템 추가
	bool BuyItem(const GuildMember& requester, uint32_t shop_id, GuildInventory& inventory,
		std::shared_ptr<ItemObject> item);

	void LoadItems(std::vector<GuildShopItem> items);
	const std::vector<GuildShopItem>& GetItems() const;
	const GuildShopItem* FindShopItem(uint32_t shop_id) const;

private:
	std::vector<GuildShopItem> items_;
};
