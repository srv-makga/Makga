#pragma once

#include "common_inventory_owner.h"
#include "common_item_container.hpp"

class CommonItemObject;
class CommonInventoryOwner;

class CommonInventory
{
public:
	CommonInventory(eInvenType _type, CommonInventoryOwner* _owner)
		: m_type(_type)
		, m_owner(_owner)
	{
	}
	virtual ~CommonInventory() = default;

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	virtual void LoadDB() = 0;
	virtual void SaveDB() = 0;
	virtual void SendClient() = 0;

	virtual Count_t MaxSlot() const = 0;
	virtual Count_t UsingSlot() const = 0;
	virtual Count_t FreeSlot() const = 0;

	virtual Result_t CanAddItem(ItemIdx_t _item_index, StackCount_t _item_count) = 0;
	virtual Result_t AddItem(ItemIdx_t _item_index, StackCount_t _item_count) = 0;

	virtual Result_t CanSubItem(ItemIdx_t _item_index, StackCount_t _item_count) = 0;
	virtual Result_t SubItem(ItemIdx_t _item_index, StackCount_t _item_count) = 0;

	// @return 삭제된 아이템 Stack 갯수 반환
	virtual StackCount_t DeleteItem(ItemUid_t _item_uid) = 0;
	virtual StackCount_t DeleteItem(ItemIdx_t _item_idx) = 0;

	virtual Result_t InsertObject(CommonItemObject* _item_object) = 0;
	virtual bool EraseObject(CommonItemObject* _item_object) = 0;
	virtual CommonItemObject* FindObject(ItemUid_t _item_uid) = 0;
	virtual CommonItemObject* FindObject(ItemIdx_t _item_idx) = 0;

public:
	eInvenType InvenType() const { return m_type; }
	CommonInventoryOwner* Owner() const { return m_owner; }

protected:
	eInvenType m_type;
	CommonInventoryOwner* m_owner;
	CommonItemContainer<ItemUid_t, CommonItemObject*> m_items;
};