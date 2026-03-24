#pragma once

#include "item_object.h"

#include <functional>
#include <memory>
#include <vector>

import makga.lib.lock;

// @brief 슬롯 기반 인벤토리 기반 클래스.
// 유저 인벤토리·길드 인벤토리가 공통으로 상속하여 사용.
class Inventory
{
public:
	static constexpr std::size_t DefaultMaxSlots = 100;

	explicit Inventory(std::size_t max_slots = DefaultMaxSlots);
	virtual ~Inventory() = default;

	// @brief 아이템 추가. 동일 ItemIdx 스택 병합 후 빈 슬롯에 배치.
	bool AddItem(std::shared_ptr<ItemObject> item);

	// @brief uid 기준 아이템에서 count만큼 차감 (0이 되면 슬롯 제거)
	bool RemoveItem(ItemUid uid, StackCount count = 1);

	// @brief item_idx 기준 스택 차감 (스택 아이템용)
	bool RemoveItem(ItemIdx item_idx, StackCount count);

	std::shared_ptr<ItemObject> GetItem(ItemUid uid) const;
	std::shared_ptr<ItemObject> FindItem(ItemIdx item_idx) const;

	std::size_t GetItemCount(ItemIdx item_idx) const;
	bool HasItem(ItemIdx item_idx, StackCount count = 1) const;

	bool IsEmpty() const;
	bool IsFull() const;
	std::size_t GetUsedSlots() const;
	std::size_t GetMaxSlots() const;

	void ForEachItem(std::function<void(std::shared_ptr<ItemObject>)> func) const;

protected:
	std::size_t max_slots_;
	std::vector<std::shared_ptr<ItemObject>> slots_;   // null = 빈 슬롯
	mutable makga::lib::SharedMutex inventory_mutex_;
};
