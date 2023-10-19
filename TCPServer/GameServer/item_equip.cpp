#include "pch.h"
#include "item_equip.h"

ItemUid_t ItemEquip::Uid() const
{
	return m_uid;
}

void ItemEquip::SetUid(ItemUid_t _uid)
{
	m_uid = _uid;
}

StackCount_t ItemEquip::StackCount() const
{
	return 1;
}