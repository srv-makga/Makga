#pragma once

#include <atomic>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>
#include <optional>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WIN32_WINNT 0x0A00

// ��ŷ Ws2_32.lib (�� �ʿ��ϸ� Mswsock.lib

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>
#include <ws2def.h>
#include <ws2ipdef.h>
#include <mstcpip.h>

#include "../common/flatbuffer/enum_generated.h"
#include "../common/flatbuffer/result_generated.h"
#include "../common/flatbuffer/game_packet_generated.h"
#include "../common/flatbuffer/game_packet_id_generated.h"
#include "../common/flatbuffer/login_packet_generated.h"
#include "../common/flatbuffer/login_packet_id_generated.h"


#include "define.h"
#include "game_world.h"
#include "game_pools.h"

import makga.lib.logger;
using Logger = makga::lib::MakgaLogger;
