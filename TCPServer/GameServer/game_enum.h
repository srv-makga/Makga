#pragma once

enum eActorSearchFilter
{
	FilterCharacter = 1 << fb::eActorType_Character,
	FilterMonster = 1 << fb::eActorType_Monster,
	FilterObject = 1 << fb::eActorType_Object,
	FilterAll = std::numeric_limits<uint32_t>::max()
};