#pragma once

struct ItemTableInfo;
class ItemBase;

class ItemProperty
{
public:
	ItemProperty(ItemTableInfo& _info);
	~ItemProperty();

	ItemIdx_t ItemIndex() const;
	StackCount_t StackCount() const;
	bool IsStack() const;
	fb::eItemType ItemType() const;

public:
	ItemBase* AllocItem() const;

private:
	ItemTableInfo& m_info;
};
