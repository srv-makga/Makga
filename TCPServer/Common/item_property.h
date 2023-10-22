#pragma once

#include "common_header.h"
#include "data_struct.h"

class ItemProperty
{
public:
	ItemProperty(ItemTableInfo* _info);
	~ItemProperty();

	ItemIdx_t ItemIndex() const;
	StackCount_t MaxStack() const;
	bool IsStack() const;
	fb::eItemType ItemType() const;

private:
	ItemTableInfo* m_info;
};
