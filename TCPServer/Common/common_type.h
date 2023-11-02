#pragma once

#include "common_const.h"
#include "FlatBuffer.h"
#include "packet.h"

#include "../Core/core_type.h"
#include "../Core/string.hpp"
#include "../Core/vector2.hpp"
#include "../Core/vector3.hpp"
#include "../Core/io_context.hpp"

#define FB_BUILDER					flatbuffers::FlatBufferBuilder
#define CREATE_FBB(fbb)				flatbuffers::FlatBufferBuilder fbb;

#define SERVER_ID_TO_WORLD_ID(server_id)	(server_id / 1000000);
#define SERVER_ID_TO_TYPE(server_id)		static_cast<fb::eServerType>((server_id % 1000000) / 1000)

/*
	메모리 최적화를 위한 데이터만 크기를 지정하고
	서버 내에서 쓰는 변수는 되도록 64bit형 타입을 지향

	사칙연산 있는 건 signed
	사칙연산 없는 건 unsigned

	uid = 고유한 번호
	idx = 테이블 번호
	id = 서버 내부 번호
*/

// core 관련 재정의
using String8 = core::string::String<char>;
using String16 = core::string::String<wchar_t>;
using NetPacket = Packet;
using NetPacketHeader = NetPacket::PacketHeader;
using IOContext_t = core::network::IOContext<>;
using IOType_t = core::network::IOType;

// flatbuffer 관련 재정의
using PacketId_t = uint16_t;
using fbb = flatbuffers::FlatBufferBuilder;

using Result_t = fb::eResult;


// Network
using Port_t = uint16_t;
using SessionId_t = uint64_t;
using ThreadId_t = std::size_t;

// System
using WorldId_t = uint32_t;
using ServerId_t = uint32_t;
using ServerType_t = fb::eServerType;
using TableIdx_t = uint32_t;
using Count_t = int32_t;
using Time_t = std::time_t;

// User
using Account_t = String8;
using UserUid_t = uint64_t;

// Map
using TerrainIdx_t = TableIdx_t;
using TerrainUid_t = uint32_t;

// Coord
using Coord_t = float;
using CoordPoint_t = std::pair<Coord_t, Coord_t>;
using Vector_t = core::coord::Vector3<Coord_t>;

// Money
using Currency_t = int64_t;
using Gold_t = Currency_t;

// Actor
using ActorId_t = uint64_t;
using Hp_t = int64_t;
using Mp_t = int64_t;
using Level_t = int32_t;
using Exp_t = int64_t;

using Attack_t = int64_t;
using Defence_t = int64_t;
using Speed_t = int32_t;
using ChaosPoint_t = int64_t;

// Character
using CharUid_t = uint64_t;
using CharIdx_t = TableIdx_t;

// Monster
using MonsterIdx_t = TableIdx_t;

// Object
using ObjectIdx_t = TableIdx_t;

using AbilityIdx_t = TableIdx_t;
using AbilityValue_t = int32_t;

// Item
using InventoryCount_t = std::size_t;
using ItemUid_t = uint64_t;
using ItemIdx_t = TableIdx_t;
using StackCount_t = int64_t;
using Reinforce_t = int32_t; // 강화
using Durability_t = int32_t; // 내구도

// Guild
using GuildUid_t = uint64_t;

// Skill
using SkillIdx_t = TableIdx_t;

using Rate_t = float;
using Percent_t = int32_t;


class StringToEnumException : public std::exception
{
public:
	const char* what()
	{
		return "Not found Enum";
	}
};

#define STR_TO_ENUM(ENUMTYPE, str)					StrTo##ENUMTYPE(str)

#define GET_ENUM_NAMES(ENUMTYPE)					fb::EnumNames##ENUMTYPE
#define GET_ENUM_VALUES(ENUMTYPE)					fb::EnumValues##ENUMTYPE
#define DEFINE_STR_TO_ENUM_TITLE(ENUMTYPE)			static fb::ENUMTYPE StrTo##ENUMTYPE##(const std::string& _str)
#define DEFINE_STR_TO_ENUM(ENUMTYPE)				DEFINE_STR_TO_ENUM_TITLE(ENUMTYPE)										\
													{																		\
														auto strArray = GET_ENUM_NAMES(ENUMTYPE)();							\
														for (int i = 0; nullptr != *strArray; ++i, ++strArray)				\
														{																	\
															if (0 == _str.compare(*strArray))								\
															{																\
																return GET_ENUM_VALUES(ENUMTYPE)()[i];						\
															}																\
														}																	\
														throw StringToEnumException();										\
													}


DEFINE_STR_TO_ENUM(eServerType);