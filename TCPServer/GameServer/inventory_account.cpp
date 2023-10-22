#include "pch.h"
#include "inventory_account.h"
#include "../Common/item_manager.h"
#include "item_object_base.h"
#include "item_property.h"
#include "user.h"

InventoryAccount::InventoryAccount(eInvenType _type, User* _owner)
	: InventoryBase(_type, _owner)
{
}

InventoryAccount::~InventoryAccount()
{
}

void InventoryAccount::Initialize()
{
}

void InventoryAccount::Finalize()
{
}

void InventoryAccount::LoadDB()
{
}

void InventoryAccount::SaveDB()
{
}

void InventoryAccount::SendClient()
{
}

Count_t InventoryAccount::MaxSlot() const
{
	return Count_t();
}

Count_t InventoryAccount::UsingSlot() const
{
	return m_items.Size();
}

Count_t InventoryAccount::FreeSlot() const
{
	return MaxSlot() - UsingSlot();
}

Result_t InventoryAccount::CanAddItem(ItemIdx_t _item_index, StackCount_t _item_count)
{
	const ItemProperty* item_property = ITEM_MANAGER.Find(_item_index);
	if (nullptr == item_property)
	{
		return eResult_InvalidIndex;
	}

	return CanAddItem(*item_property, _item_count);
}

Result_t InventoryAccount::CanAddItem(const ItemProperty& _item_property, StackCount_t _item_count)
{
	ItemObjectBase* item_object = FindObject(_item_property.ItemIndex());
	StackCount_t cur_stack = item_object ? item_object->Stack() : 0;

	if (_item_property.MaxStack() - cur_stack < _item_count)
	{
		return eResult_ItemStackFull;
	}
	
	return eResult_Success;
}

Result_t InventoryAccount::AddItem(ItemIdx_t _item_index, StackCount_t _item_count, bool _client_send)
{
	auto* item_property = ITEM_MANAGER.Find(_item_index);
	if (nullptr == item_property)
	{
		return eResult_InvalidIndex;
	}

	return AddItem(*item_property, _item_count, _client_send);
}

Result_t InventoryAccount::AddItem(const ItemProperty& _item_property, StackCount_t _item_count, bool _client_send)
{
	Result_t result = CanAddItem(_item_property, _item_count);
	if (eResult_Success != result)
	{
		return result;
	}

	auto item_object = FindObject(_item_property.ItemIndex());
	if (nullptr != item_object)
	{
		item_object->AddStack(_item_count);
		ReflectionObject(ReflectType::UpdateStack, item_object);
	}
	else
	{
		// @todo alloc object
		item_object->SetStack(_item_count);

		result = InsertObject(item_object);
		if (eResult_Success != result)
		{
			// @todo return object
			return result;
		}

		ReflectionObject(ReflectType::Insert, item_object);
	}

	return eResult_Success;
}

Result_t InventoryAccount::AddItem(ItemObjectBase* _item_object, bool _client_send)
{
	return Result_t();
}

Result_t InventoryAccount::CanSubItem(ItemIdx_t _item_index, StackCount_t _item_count)
{
	return Result_t();
}

Result_t InventoryAccount::SubItem(ItemIdx_t _item_index, StackCount_t _item_count)
{
	return Result_t();
}

StackCount_t InventoryAccount::DeleteItem(ItemUid_t _item_uid)
{
	ItemObjectBase* item_object = FindObject(_item_uid);
	return item_object ? item_object->Stack() : 0;
}

StackCount_t InventoryAccount::DeleteItem(ItemIdx_t _item_idx)
{
	return StackCount_t();
}

Result_t InventoryAccount::InsertObject(ItemObjectBase* _item_object)
{
	return Result_t();
}

bool InventoryAccount::EraseObject(ItemObjectBase* _item_object)
{
	return false;
}

ItemObjectBase* InventoryAccount::FindObject(ItemUid_t _item_uid)
{
	return nullptr;
}

ItemObjectBase* InventoryAccount::FindObject(ItemIdx_t _item_idx)
{
	return nullptr;
}