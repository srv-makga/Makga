#include "pch.h"
#include "inventory_account.h"
#include "item_manager.h"
#include "item_property.h"

InventoryAccount::InventoryAccount()
{
}

InventoryAccount::~InventoryAccount()
{
}

void InventoryAccount::Initialize()
{
	InventoryBase::Initialize();
}

void InventoryAccount::InitOwner(InventoryOwner* _owner)
{
	SetOwner(_owner);
}

void InventoryAccount::Finalize()
{
}

Result_t InventoryAccount::Add(ItemIdx_t _item_idx, StackCount_t _stack_count)
{
	ItemProperty* item_property = ITEM_MANAGER.Find(_item_idx);
	if (nullptr == item_property)
	{
		return eResult_InvalidIndex;
	}

	if (true == item_property->IsStack())
	{
		StackCount_t fill_count = CanFillStackCount(*item_property);
		InventoryCount_t need_count = (_stack_count - fill_count) / item_property->StackCount();
		if (need_count * item_property->StackCount() != (_stack_count - fill_count))
		{
			++need_count;
		}

		if (MaxCount() - Count() < need_count)
		{
			return eResult_InvenFull;
		}

		StackCount_t remain_stack_count = FillStackCount(*item_property, _stack_count);
		if (0 < remain_stack_count)
		{
			// @todo
		}
	}
	else
	{
		if (MaxCount() - Count() < _stack_count)
		{
			return eResult_InvenFull;
		}
	}

	// @todo 추가 작업
	
	return eResult_Success;
}

Result_t InventoryAccount::Add(ItemObject* _itemObject)
{
    return Result_t();
}

Result_t InventoryAccount::Add(std::unordered_map<ItemIdx_t, StackCount_t>& _in_out)
{
    return Result_t();
}

ItemObject* InventoryAccount::Find(ItemUid_t _item_uid) const
{
    return nullptr;
}

ItemObject* InventoryAccount::Find(ItemIdx_t _item_idx) const
{
    return nullptr;
}

Result_t InventoryAccount::Remove(ItemUid_t _item_uid)
{
    return Result_t();
}

Result_t InventoryAccount::Remove(ItemIdx_t _item_idx, StackCount_t _count)
{
    return Result_t();
}

InventoryCount_t InventoryAccount::Count() const
{
	return m_container.size();
}

StackCount_t InventoryAccount::CanFillStackCount(const ItemProperty& _item_property) const
{
	if (false == _item_property.IsStack())
	{
		return 0;
	}

	auto [start, end] = m_container_by_index.equal_range(_item_property.ItemIndex());
	if (m_container_by_index.end() == start)
	{
		return 0;
	}

	StackCount_t stack_sum = 0;

	for (auto iter = start; end != start; ++start)
	{
		stack_sum += (_item_property.StackCount() - start->second->StackCount());
	}

	return stack_sum;
}

StackCount_t InventoryAccount::FillStackCount(const ItemProperty& _item_property, StackCount_t _stack_count) const
{
	return StackCount_t();
}
