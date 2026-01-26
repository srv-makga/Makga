#pragma once

#include "pch.h"

struct MapTable
{
	TerrainIdx idx = 0;

};


struct MonsterTable
{
	MonsterIdx idx = 0;

	makga::AIType ai_type = makga::AIType::AIType_NonAggressive;
};