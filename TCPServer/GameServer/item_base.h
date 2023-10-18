#pragma once

#include "../Common/common_struct.h"
#include "item_property.h"

class ItemBase
{
public:
	ItemBase() = default;

	void Initialize();
	void InitProperty(const ItemProperty* _property);

public: // property ����
	ItemIdx_t Index() const;
	fb::eItemType Type() const;

public: // ������ ��ü ����
	virtual ItemUid_t Uid() const = 0;
	virtual void SetUid(ItemUid_t _uid) = 0;

	virtual StackCount_t StackCount() const = 0;
	virtual void SetStackCount(StackCount_t _count) = 0;
	// @brief ��ø �ִ�ġ���� ���� ����
	virtual StackCount_t CanFillStackCount() = 0;
	// @brief ��ø �ִ�ġ�� ����
	virtual void FillMaxStackCount() = 0;

private:
	const ItemProperty* m_property;
};