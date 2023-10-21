#pragma once

#include "common_header.h"

class ItemObjectBase
{
public:
	ItemObjectBase() = default;
	virtual ~ItemObjectBase() = default;

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	virtual ItemUid_t ItemUid() const = 0;
	virtual ItemIdx_t ItemIndex() const = 0;
	virtual StackCount_t StackCount() const = 0;
};