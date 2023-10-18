#pragma once

#include "inventory_base.h"
#include "inventory_controller.h"
#include <unordered_map>

class InventoryAccount : public InventoryBase, public InventoryController
{
public:
	using Container_t = std::unordered_map<ItemUid_t, ItemBase*>;
	using CacheContainer_t = std::unordered_multimap<ItemIdx_t, ItemBase*>;

public:
	InventoryAccount();
	virtual ~InventoryAccount();

	void Initialize();
	void InitOwner(InventoryOwner* _owner);
	void Finalize();

	Result_t Add(ItemIdx_t _item_idx, StackCount_t _stack_count) override;
	Result_t Add(ItemObject* _itemObject) override;
	Result_t Add(std::unordered_map<ItemIdx_t, StackCount_t>& _in_out) override;

	ItemObject* Find(ItemUid_t _item_uid) const override;
	ItemObject* Find(ItemIdx_t _item_idx) const override;

	Result_t Remove(ItemUid_t _item_uid) override;
	Result_t Remove(ItemIdx_t _item_idx, StackCount_t _count = 1) override;

	InventoryCount_t Count() const override;

public: // ��ü ���
	// @brief �߰� �κ� ��� ���� ��ø ������ ����
	StackCount_t CanFillStackCount(const ItemProperty& _item_property) const;

	// @brief �߰� �κ� ��� ���� ��ø ó��
	// @return ���� ��ø ��
	StackCount_t FillStackCount(const ItemProperty& _item_property, StackCount_t _stack_count) const;
	
public:
	Container_t m_container;
	// ĳ�� ������
	CacheContainer_t m_container_by_index;
};