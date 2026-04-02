#pragma once

#include "guild_member.h"
#include "../common/inventory.h"

// @brief 길드 인벤토리. Inventory를 상속받아 등급별 접근 제어와 1일 인출 한도를 추가.
class GuildInventory : public Inventory
{
public:
	struct WithdrawPolicy
	{
		StackCount master_limit = -1; // -1 = 무제한
		StackCount sub_master_limit = 100;
		StackCount member_limit = 20;
	};

	explicit GuildInventory(std::size_t max_slots = 200);

	// @brief 아이템 입고 (기본: Master/SubMaster만 가능)
	bool Deposit(const GuildMember& requester, std::shared_ptr<ItemObject> item);

	// @brief 아이템 출고 — 등급별 1일 한도 체크 후 성공 시 유저 인벤 이동 신호 반환.
	// 실제 인벤토리 이동은 GameServer / ItemService가 처리.
	bool Withdraw(const GuildMember& requester, ItemIdx item_idx, StackCount count);

	void SetPolicy(const WithdrawPolicy& policy);

	// @brief 자정 타이머에서 호출하여 당일 인출량 초기화
	void ResetDailyWithdrawals();

private:
	StackCount get_limit(GuildGrade grade) const;

	WithdrawPolicy policy_;
	std::unordered_map<UserUid, StackCount> today_withdrawn_;
	std::mutex withdraw_mutex_;
};
