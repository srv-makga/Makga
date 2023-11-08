#pragma once

#include "common_struct.h"

#define DECLARE_PROC_FUNCTION(name)		bool On##name(NetPacket* _packet);


#define RETURN_RESULT(msg, result)		LOG_ERROR << msg << LOG_RESULT(result); return result;