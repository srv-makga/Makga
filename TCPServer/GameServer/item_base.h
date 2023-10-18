#pragma once

#include "../Common/common_struct.h"
#include "item_property.h"

class ItemBase
{
public:
	ItemBase() = default;

	void Initialize();
	void InitProperty(const ItemProperty* _property);

public: // property 랩핑
	ItemIdx_t Index() const;
	fb::eItemType Type() const;

public: // 아이템 자체 정보
	virtual ItemUid_t Uid() const = 0;
	virtual void SetUid(ItemUid_t _uid) = 0;

	virtual StackCount_t StackCount() const = 0;
	virtual void SetStackCount(StackCount_t _count) = 0;
	// @brief 중첩 최대치까지 남은 갯수
	virtual StackCount_t CanFillStackCount() = 0;
	// @brief 중첩 최대치로 설정
	virtual void FillMaxStackCount() = 0;

private:
	const ItemProperty* m_property;
};