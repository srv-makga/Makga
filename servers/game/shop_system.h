#pragma once

#include <optional>
#include <unordered_map>
#include <vector>

#include "../common/define.h"
#include "../common/data_struct.h"

import makga.lib.pattern.singleton;
import makga.lib.lock;

// @brief NPC 상점 아이템 캐시 (DataTable ShopItemTable 포인터 미러)
struct ShopData
{
TableIdx                          shop_idx  = 0;
std::string                       shop_name;
TableIdx                          npc_id    = 0;
std::vector<const ShopItemTable*> items;   // DataTable 소유 — 포인터만 참조
};

// @brief 상점 시스템
//   OpenShop  : NPC 상점 데이터 조회 → S2C_ShopList 전송
//   BuyItem   : 골드·레벨 검증 → 아이템 생성 → 인벤토리 추가 → S2C_ShopResult
//   SellItem  : 인벤토리 제거 → 골드 지급 → S2C_ShopResult
class ShopSystem final : public makga::lib::Singleton<ShopSystem>
{
public:
ShopSystem()  = default;
~ShopSystem() = default;

bool Initialize();
void Finalize();

// @brief actor_id 플레이어에게 npc_idx NPC 상점 목록을 전송한다.
void OpenShop(ActorId actor_id, TableIdx npc_idx);

// @brief shop_item_idx 아이템을 count개 구매한다.
bool BuyItem(ActorId actor_id, TableIdx shop_idx,
             TableIdx shop_item_idx, StackCount count);

// @brief item_uid 아이템을 count개 판매한다.
bool SellItem(ActorId actor_id, ItemUid item_uid, StackCount count);

// @brief npc_idx에 해당하는 캐시 ShopData 반환 (없으면 nullopt)
std::optional<ShopData> GetShopData(TableIdx npc_idx) const;

private:
void BuildNpcShopIndex();

mutable makga::lib::SharedMutex            mutex_;
std::unordered_map<TableIdx, ShopData>     npc_shop_cache_;  // npc_id → ShopData
};
