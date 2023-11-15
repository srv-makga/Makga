#include "pch.h"
#include "inventory_account.h"
#include "../Common/item_manager.h"
#include "item_object_base.h"
#include "item_property.h"
#include "user.h"

InventoryAccount::InventoryAccount(eInvenType _type, User* _owner)
	: InventoryBase(_type, _owner)
	, m_user(nullptr)
{
}

InventoryAccount::~InventoryAccount()
{
	Finalize();
}

void InventoryAccount::Initialize()
{
	std::vector<ItemObjectBase*> temp;
	m_items.FetchAll(temp);

	for (auto object : temp)
	{
		object->Finalize();
	}

	m_items.Initialize();
	m_container_by_index.clear();
	m_delay_reflection.clear();
}

void InventoryAccount::Finalize()
{
	std::vector<ItemObjectBase*> temp;
	m_items.FetchAll(temp);

	for (auto object : temp)
	{
		object->Finalize();
	}

	m_items.Initialize();
	m_container_by_index.clear();
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
	return Owner()->MaxInvenSlot();
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
	if (nullptr == _item_object)
	{
		return eResult_InvalidParameter;
	}

	Result_t result = CanAddItem(*_item_object->Property(), _item_object->Stack());
	if (eResult_Success != result)
	{
		return result;
	}

	// 우선 동일한 인덱스는 단일 오브젝트룰
	if (nullptr != FindObject(_item_object->Index()))
	{
		return eResult_ItemIdxDuplicate;
	}

	result = InsertObject(_item_object);
	if (eResult_Success != result)
	{
		return result;
	}

	ReflectionObject(ReflectType::Insert, _item_object);

	return eResult_Success;
}

Result_t InventoryAccount::CanSubItem(ItemIdx_t _item_index, StackCount_t _item_count)
{
	auto item_property = ITEM_MANAGER.Find(_item_index);
	if (nullptr == item_property)
	{
		return eResult_InvalidIndex;
	}

	return CanSubItem(*item_property, _item_count);
}

Result_t InventoryAccount::CanSubItem(const ItemProperty& _item_property, StackCount_t _item_count)
{
	auto item_object = FindObject(_item_property.ItemIndex());
	if (nullptr == item_object)
	{
		return eResult_ItemNotFound;
	}

	if (item_object->Stack() < _item_count)
	{
		return eResult_ItemNotEnough;
	}

	return eResult_Success;
}

Result_t InventoryAccount::SubItem(ItemIdx_t _item_index, StackCount_t _item_count)
{
	auto item_property = ITEM_MANAGER.Find(_item_index);
	if (nullptr == item_property)
	{
		return eResult_InvalidIndex;
	}

	return SubItem(*item_property, _item_count);
}

Result_t InventoryAccount::SubItem(const ItemProperty& _item_property, StackCount_t _item_count)
{
	if (0 == ObjectCount(_item_property.ItemIndex()))
	{
		return eResult_ItemNotEnough;
	}

	if (true == _item_property.IsStack())
	{
		auto item_object = FindObject(_item_property.ItemIndex());
		if (item_object->Stack() < _item_count)
		{
			return eResult_ItemNotEnough;
		}

		if (item_object->Stack() == _item_count)
		{
			EraseObject(item_object);
			ReflectionObject(ReflectType::Delete, item_object);
			item_object->Finalize();
		}
		else
		{
			item_object->SubStack(_item_count);
			ReflectionObject(ReflectType::UpdateStack, item_object);
		}
	}
	else
	{
		auto iter = m_container_by_index.find(_item_property.ItemIndex());
		auto s = iter->second.begin();

		for (StackCount_t i = 0; i < _item_count; ++i)
		{
			s = iter->second.erase(s);
		}
	}

	return eResult_Success;
}

Count_t InventoryAccount::ObjectCount(ItemIdx_t _item_index)
{
	auto iter = m_container_by_index.find(_item_index);
	if (m_container_by_index.end() == iter)
	{
		return 0;
	}

	return iter->second.size();
}

bool InventoryAccount::HasItemIdx(ItemIdx_t _item_index)
{
	return 0 != ObjectCount(_item_index);
}

Result_t InventoryAccount::InsertObject(ItemObjectBase* _item_object)
{
	if (nullptr == _item_object)
	{
		return eResult_InvalidParameter;
	}

	if (true == m_items.Has(_item_object->Uid()))
	{
		return eResult_ItemUidDuplicate;
	}

	if (false == m_items.Insert(_item_object->Uid(), _item_object))
	{
		return eResult_ItemUidDuplicate;
	}

	m_container_by_index[_item_object->Index()].insert(_item_object);

	return eResult_Success;
}

bool InventoryAccount::EraseObject(ItemObjectBase* _item_object)
{
	if (nullptr == _item_object)
	{
		return eResult_InvalidParameter;
	}

	if (false == m_items.Has(_item_object->Uid()))
	{
		return eResult_ItemNotFound;
	}

	if (false == m_items.Erase(_item_object->Uid()))
	{
		return eResult_ItemNotFound;
	}

	m_container_by_index[_item_object->Index()].erase(_item_object);

	return eResult_Success;
}

ItemObjectBase* InventoryAccount::FindObject(ItemUid_t _item_uid)
{
	return m_items.Find(_item_uid);
}

ItemObjectBase* InventoryAccount::FindObject(ItemIdx_t _item_idx)
{
	auto iter = m_container_by_index.find(_item_idx);
	if (m_container_by_index.end() == iter)
	{
		return nullptr;
	}

	return *iter->second.begin();
}