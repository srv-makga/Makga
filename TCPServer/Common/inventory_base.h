#pragma once

#include "item_base.h"

class InventoryOwner
{
public:
	InventoryOwner() = default;
	virtual ~InventoryOwner() = default;

	virtual InventoryCount_t InventoryCount() const = 0;
};

// @brief 인벤토리 인터페이스격 클래스

class InventoryBase
{
public:
	InventoryBase() = default;
	virtual ~InventoryBase() = default;

	void Initialize() { m_owner = nullptr; }
	virtual InventoryCount_t Count() const = 0;
	InventoryCount_t MaxCount() const { return m_owner ? m_owner->InventoryCount() : 0; }

protected:
	InventoryOwner* Owner() const { return m_owner; }
	void SetOwner(InventoryOwner* _owner) { m_owner = _owner; }

private:
	InventoryOwner* m_owner;
};