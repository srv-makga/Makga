#pragma once

#include "common_header.h"

class InventoryOwner
{
public:
	InventoryOwner() = default;
	virtual ~InventoryOwner() = default;

	virtual uint64_t OwnerUid() const = 0;
	virtual int32_t MaxInvenSlot() const = 0;

	virtual eResult GiveItem(ItemIdx_t _item_idx, StackCount_t _item_stack, bool _is_send_client) = 0;
	virtual eResult GiveItem(const ItemProperty& _item_property, StackCount_t _item_stack, bool _is_send_client) = 0;
	virtual eResult GiveItem(UmapItemList& _item_list, bool _is_send_client) = 0;
	virtual eResult GiveItem(UmapItemPropertyList& _item_list, bool _is_send_client) = 0;
};