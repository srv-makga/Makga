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
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <atomic>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <utility>
#include <sstream>
#include <fstream>
#include <set>
#include <map>
#include <bitset>
#include <limits>
#include <numeric>
#include <optional>
#include <variant>
#include <filesystem>
#include <tuple>

#include "../Core/logger.h"
#include "game_header.h"

#include "flatbuffers/flatbuffers.h"
#include "rapidjson/rapidjson.h"