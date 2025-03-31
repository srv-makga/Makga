#pragma once

#include "item_object_base.h"
#include "inventory_owner.h"
#include "item_container_base.hpp"

class InventoryOwner;
class ItemProperty;

enum class ReflectType
{
	Insert,
	UpdateStack,
	UpdateSkin,
	UpdateReinforce,
	Delete
};

class InventoryBase
{
public:
	using Items = ItemContainerBase<ItemUid_t, ItemObjectBase*>;

public:
	InventoryBase(eInvenType _type, InventoryOwner* _owner)
		: m_type(_type)
		, m_owner(_owner)
	{
	}
	virtual ~InventoryBase() = default;

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	virtual void LoadDB() = 0;
	virtual void SaveDB() = 0;
	virtual void SendClient() = 0;

	virtual Count_t MaxSlot() const = 0;
	virtual Count_t UsingSlot() const = 0;
	virtual Count_t FreeSlot() const = 0;

	virtual Result_t CanAddItem(ItemIdx_t _item_index, StackCount_t _item_count) = 0;
	virtual Result_t CanAddItem(const ItemProperty& _item_property, StackCount_t _item_count) = 0;
	virtual Result_t CanAddItem(std::unordered_map<ItemIdx_t, StackCount_t>& _list) = 0;

	virtual Result_t CanSubItem(ItemIdx_t _item_index, StackCount_t _item_count) = 0;
	virtual Result_t CanSubItem(const ItemProperty& _item_property, StackCount_t _item_count) = 0;
	virtual Result_t CanSubItem(std::unordered_map<ItemIdx_t, StackCount_t>& _list) = 0;

	virtual Result_t AddItem(ItemIdx_t _item_index, StackCount_t _item_count, bool _client_send) = 0;
	virtual Result_t AddItem(const ItemProperty& _item_property, StackCount_t _item_count, bool _client_send) = 0;
	virtual Result_t AddItem(ItemObjectBase* _item_object, bool _client_send) = 0;

	// @brief 老何 昏力
	virtual Result_t SubItem(ItemIdx_t _item_index, StackCount_t _item_count) = 0;
	virtual Result_t SubItem(const ItemProperty& _item_property, StackCount_t _item_count) = 0;

	// @brief 傈眉 昏力
	virtual Result_t DeleteItem(ItemUid_t _item_uid) = 0;
	virtual Result_t DeleteItem(ItemIdx_t _item_index) = 0;

	virtual Count_t ObjectCount(ItemIdx_t _item_index) = 0;
	virtual bool HasItemIdx(ItemIdx_t _item_index) = 0;

	virtual Result_t InsertObject(ItemObjectBase* _item_object) = 0;
	virtual Result_t EraseObject(ItemObjectBase* _item_object) = 0;
	virtual ItemObjectBase* FindObject(ItemUid_t _item_uid) = 0;
	virtual ItemObjectBase* FindObject(ItemIdx_t _item_idx) = 0;

public:
	void ReflectionObject(ReflectType _type, ItemObjectBase* _item_object)
	{
		_item_object->Reflection(_type, this);
	}
	eInvenType InvenType() const { return m_type; }
	InventoryOwner* Owner() const { return m_owner; }

protected:
	eInvenType m_type;
	InventoryOwner* m_owner;
};