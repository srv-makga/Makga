#pragma once

#include "common_struct.h"

#define DECLARE_PROC_FUNCTION(name)		bool On##name(NetPacket* _packet);