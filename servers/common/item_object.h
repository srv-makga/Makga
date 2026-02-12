#pragma once

#include "define.h"

// @brief 아이템 인터페이스
class ItemObject
{
public:
	ItemObject() = default;
	virtual ~ItemObject() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;

	virtual makga::Result Use(StackCount use_count = 1) = 0;

public:
	virtual StackCount AddStackCount(StackCount count) = 0;
	virtual StackCount SubStackCount(StackCount count) = 0;

	virtual RefineLevel GetRefineLevel() const = 0;
	virtual void SetRefineLevel(RefineLevel level) = 0;

public:
	inline ItemUid GetUid() const { return uid_; }
	inline void SetUid(ItemUid uid) { uid_ = uid; }

	inline ItemIdx GetIdx() const { return item_idx_; }
	inline void SetIdx(ItemIdx idx) { item_idx_ = idx; }

	inline StackCount GetStackCount() const { return stack_count_; }
	inline void SetStackCount(StackCount count) { stack_count_ = count; }

	inline bool IsLock() const { return is_locked_; }
	inline void SetLock(bool is_lock) { is_locked_ = is_lock; }

	inline Time GetExpireTime() const { return expire_time_; }
	inline void SetExpireTime(Time expire_time) { expire_time_ = expire_time; }

protected:
	ItemUid uid_;
	ItemIdx item_idx_;
	StackCount stack_count_;
	bool is_locked_;
	Time expire_time_;
};