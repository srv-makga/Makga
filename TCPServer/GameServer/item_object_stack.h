#pragma once

#include "../Common/item_object_base.h"

class ItemObjectStack : public ItemObjectBase
{
public:
	ItemObjectStack();
	virtual ~ItemObjectStack();

	void Initialize() override;
	void Finalize() override;

	void SetInfo(std::string&& _json) override;

	ItemUid_t Uid() const override;
	void SetUid(ItemUid_t _uid) const override;

	ItemIdx_t Index() const override;
	void SetIndex(ItemIdx_t _index) const override;

	StackCount_t Stack() const override;
	void SetStack(StackCount_t _stack) const override;

private:
	ItemUid_t m_uid;
};