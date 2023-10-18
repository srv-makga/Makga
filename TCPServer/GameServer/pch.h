#pragma once

#pragma warning (disable:4786)
#pragma warning (disable:4259)
#pragma warning (disable:4995)
#pragma warning (disable:4996)
#pragma warning (disable:4005)

#pragma comment ( lib, "winmm.lib" )
#pragma comment ( lib, "ws2_32.lib" )
#pragma comment ( lib, "Iphlpapi.lib" )

#ifdef _USE_32BIT_TIME_T
	#undef _USE_32BIT_TIME_T
#endif

// Windows Header Files:
#define NOMINMAX
#include <winsock2.h>
#include <windows.h>

#include "../Core/logger.h"
#include "game_header.h"

#include <cstdlib>
#include <cstdio>
#include <string>
#include <cassert>
#include <iostream>

#include "flatbuffers/flatbuffers.h"
#include "rapidjson/rapidjson.h"