#include "pch.h"
#include "item_stack.h"

ItemUid_t ItemStack::Uid() const
{
	return m_uid;
}

void ItemStack::SetUid(ItemUid_t _uid)
{
	m_uid = _uid;
}