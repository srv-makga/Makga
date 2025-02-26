#pragma once

#include "../Common/item_object_base.h"

class ItemObjectEquip : public ItemObjectBase
{
public:
	ItemObjectEquip();
	virtual ~ItemObjectEquip();

	void Initialize() override;
	void Finalize() override;

	void SetInfo(std::string&& _json) override;

	ItemUid_t ItemUid() const override;
	void SetItemUid(ItemUid_t _uid) const override;

	ItemIdx_t ItemIndex() const override;
	void SetItemIndex(ItemIdx_t _index) const override;

	StackCount_t Stack() const override;
	void SetStack(StackCount_t _stack) const override;

private:
	ItemUid_t m_uid;
};