#pragma once

#include <atomic>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WIN32_WINNT 0x0A00

// 링킹 Ws2_32.lib (및 필요하면 Mswsock.lib

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>
#include <ws2def.h>
#include <ws2ipdef.h>
#include <mstcpip.h>


#include "define.h"

import makga.lib.logger;
using Logger = makga::lib::MakgaLogger;