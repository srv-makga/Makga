#pragma once

#include "item_base.h"

class ItemStack : public ItemBase
{
public:
	ItemStack() = default;

	ItemUid_t Uid() const override;
	void SetUid(ItemUid_t _uid) override;

private:
	ItemUid_t m_uid;
};