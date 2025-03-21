#pragma once

#include "common_struct.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// macro
#define LOG_SESSION_UID(u)				" !" << u
#define LOG_USER_UID(u)					" @" << u
#define LOG_ACTOR_UID(u)				" #" << u
#define LOG_MAP_UID(u)					" $" << u
#define LOG_PARTY_UID(u)				" %" << u
#define LOG_GUILD_UID(u)				" ^" << u
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DECLARE_PROC_FUNCTION(name)		bool On##name(NetPacket* _packet);

#define RETURN_RESULT(msg, result)		LOG_ERROR << msg << LOG_RESULT(result); return result;

constexpr uint64_t strhash(const char* s, size_t index = 0)
{
	return s + index == nullptr || s[index] == '\0' ? 55 : strhash(s, index + 1) * 33 + (unsigned char)(s[index]);
}

enum AcceptorType
{
	User,
	Game,
};

enum ConnectorType
{
	World,
	Community,
	Game,
	DBAgent,
	Log,
};