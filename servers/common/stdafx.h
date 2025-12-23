#pragma once

#pragma warning(disable: 4200) // zero-sized array in struct/union
#pragma warning(disable: 4251) // DLL-interface warning
#pragma warning(disable: 4275) // non DLL-interface class used as base for DLL-interface class
#pragma warning(disable: 4996) // deprecated function warning

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib") // timeGetTime
#pragma comment(lib, "iphlpapi.lib") // GetAdaptersInfo

#define NOMINMAX

#include <WinSock2.h>
#include <Windows.h>

#include "define.h"

#include <sstream>
#include <format>
#include <future>

import makga.lib.logger;