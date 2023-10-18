#pragma once

#include "common_header.h"

class ItemObject;

class InventoryController
{
public:
	InventoryController() = default;
	virtual ~InventoryController() = default;

	virtual Result_t Add(ItemIdx_t _item_idx, StackCount_t _stack_count) = 0;
	virtual Result_t Add(ItemObject* _itemObject) = 0;
	virtual Result_t Add(std::unordered_map<ItemIdx_t, StackCount_t>& _in_out) = 0;

	virtual ItemObject* Find(ItemUid_t _item_uid) const = 0;
	virtual ItemObject* Find(ItemIdx_t _item_idx) const = 0;

	virtual Result_t Remove(ItemUid_t _item_uid) = 0;
	virtual Result_t Remove(ItemIdx_t _item_idx, StackCount_t _count = 1) = 0;
};