#pragma once

#include "common_header.h"

class CommonInventoryOwner
{
public:
	CommonInventoryOwner() = default;
	virtual ~CommonInventoryOwner() = default;

	virtual uint64_t OwnerUid() = 0;
};