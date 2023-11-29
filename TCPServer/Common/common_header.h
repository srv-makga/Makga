#pragma once

#include "common_struct.h"

#define DECLARE_PROC_FUNCTION(name)		bool On##name(NetPacket* _packet);

#define RETURN_RESULT(msg, result)		LOG_ERROR << msg << LOG_RESULT(result); return result;

constexpr uint64_t strhash(const char* s, size_t index = 0)
{
	return s + index == nullptr || s[index] == '\0' ? 55 : hashstr(s, index + 1) * 33 + (unsigned char)(s[index]);
}