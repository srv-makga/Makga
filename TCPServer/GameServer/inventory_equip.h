#pragma once

#include "inventory_base.h"

class InventoryEquip : public InventoryBase
{
public:
	using Container_t = std::unordered_map<ItemUid_t, ItemObject*>;

public:
	InventoryEquip();
	virtual ~InventoryEquip();

private:
	Container_t m_container;
};