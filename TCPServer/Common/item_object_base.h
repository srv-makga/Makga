#pragma once

#include "common_header.h"

enum class ReflectType;
class InventoryBase;
class ItemProperty;

class ItemObjectBase
{
public:
	ItemObjectBase() = default;
	virtual ~ItemObjectBase() = default;

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	virtual void SetInfo(std::string&& _json) = 0;

	virtual ItemUid_t Uid() const = 0;
	virtual void SetUid(ItemUid_t _uid) const = 0;

	virtual ItemIdx_t Index() const = 0;
	virtual void SetIndex(ItemIdx_t _index) const = 0;

	virtual StackCount_t Stack() const = 0;
	virtual void SetStack(StackCount_t _stack) const = 0;
	virtual void AddStack(StackCount_t _stack) const = 0;
	virtual void SubStack(StackCount_t _stack) const = 0;

	virtual ItemIdx_t SkinIndex() const = 0;
	virtual void SetSkinIndex(ItemIdx_t _index) const = 0;

	virtual Reinforce_t Reinforce() const = 0;
	virtual void SetReinforce(Reinforce_t _reinforce) = 0;


	virtual void Reflection(ReflectType _type, InventoryBase* _inven) = 0;

	virtual const ItemProperty* Property() const = 0;
};