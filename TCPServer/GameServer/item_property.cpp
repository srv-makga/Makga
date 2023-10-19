#include "pch.h"
#include "item_property.h"

#include "item_equip.h"
#include "item_stack.h"

ItemProperty::ItemProperty(ItemTableInfo& _info)
	: m_info(_info)
{
}

ItemProperty::~ItemProperty()
{
}

ItemIdx_t ItemProperty::ItemIndex() const
{
	return m_info.item_idx;
}

StackCount_t ItemProperty::StackCount() const
{
	return m_info.stack_count;
}

bool ItemProperty::IsStack() const
{
	return 1 < StackCount();
}

fb::eItemType ItemProperty::ItemType() const
{
	return m_info.item_type;
}

ItemBase* ItemProperty::AllocItem() const
{
	switch (ItemType())
	{
	case fb::eItemType_Weapon:
	case fb::eItemType_Helm:
	case fb::eItemType_Armor:
	case fb::eItemType_Belt:
	case fb::eItemType_Gloves:
	case fb::eItemType_Shoes:
	case fb::eItemType_Accessories:
		return new ItemEquip();
	}

    return nullptr;
}
