#pragma once

#include "common_header.h"

class InventoryOwner
{
public:
	InventoryOwner() = default;
	virtual ~InventoryOwner() = default;

	virtual uint64_t OwnerUid() const = 0;
};