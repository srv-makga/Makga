#pragma once

#include "item_base.h"

class ItemEquip : public ItemBase
{
public:
	ItemEquip() = default;

	ItemUid_t Uid() const override;
	void SetUid(ItemUid_t _uid) override;

	StackCount_t StackCount() const override;
	void SetStackCount(StackCount_t _count) override {}
	// @brief 중첩 최대치까지 남은 갯수
	StackCount_t CanFillStackCount() { return 0; }
	// @brief 중첩 최대치로 설정
	void FillMaxStackCount() {}

private:
	ItemUid_t m_uid;
};