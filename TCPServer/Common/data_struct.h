#pragma once

#include "common_struct.h"
#include <string>
#include <vector>
#include <map>

struct SystemValueInfo
{
	Gold_t resurrection_need_currency;
};

struct TerrainInfo
{
	TerrainIdx_t idx = 0;
	String8 name;

	CoordPoint_t left_top;
	CoordPoint_t right_bottom;
};

struct ItemTableInfo
{
	ItemIdx_t item_idx;
	StackCount_t stack_count;
	fb::eItemType item_type;
};