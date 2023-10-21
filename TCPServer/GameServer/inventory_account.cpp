#include "pch.h"
#include "inventory_account.h"
#include "item_manager.h"
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
	return Result_t();
}

Result_t InventoryAccount::AddItem(ItemIdx_t _item_index, StackCount_t _item_count)
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
	return StackCount_t();
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