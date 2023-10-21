#pragma once

#include "inventory_base.h"

class User;

class InventoryAccount : public InventoryBase
{
public:
	using CacheContainer_t = std::unordered_multimap<ItemIdx_t, ItemObjectBase*>;

public:
	InventoryAccount(eInvenType _type, User* _owner);
	virtual ~InventoryAccount();

	void Initialize() override;
	void Finalize() override;

	void LoadDB() override;
	void SaveDB() override;
	void SendClient() override;

	Count_t MaxSlot() const override;
	Count_t UsingSlot() const override;
	Count_t FreeSlot() const override;

	Result_t CanAddItem(ItemIdx_t _item_index, StackCount_t _item_count) override;
	Result_t AddItem(ItemIdx_t _item_index, StackCount_t _item_count) override;

	Result_t CanSubItem(ItemIdx_t _item_index, StackCount_t _item_count) override;
	Result_t SubItem(ItemIdx_t _item_index, StackCount_t _item_count) override;

	// @return 삭제된 아이템 Stack 갯수 반환
	StackCount_t DeleteItem(ItemUid_t _item_uid) override;
	StackCount_t DeleteItem(ItemIdx_t _item_idx) override;

	Result_t InsertObject(ItemObjectBase* _item_object) override;
	bool EraseObject(ItemObjectBase* _item_object) override;
	ItemObjectBase* FindObject(ItemUid_t _item_uid) override;
	ItemObjectBase* FindObject(ItemIdx_t _item_idx) override;

public: // 자체 기능

protected:
	User* GetUser() const { return m_user; }
	
private:
	CacheContainer_t m_container_by_index;
	User* m_user;
};