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
	// @brief ��ø �ִ�ġ���� ���� ����
	StackCount_t CanFillStackCount() { return 0; }
	// @brief ��ø �ִ�ġ�� ����
	void FillMaxStackCount() {}

private:
	ItemUid_t m_uid;
};