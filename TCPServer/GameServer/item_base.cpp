#include "pch.h"
#include "item_base.h"

void ItemBase::Initialize()
{
	m_property = nullptr;
}

void ItemBase::InitProperty(const ItemProperty* _property)
{
	m_property = _property;
}

ItemIdx_t ItemBase::Index() const
{
	return m_property ? m_property->ItemIndex() : 0;
}

fb::eItemType ItemBase::Type() const
{
	return m_property ? m_property->ItemType() : fb::eItemType_None;
}
