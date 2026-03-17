#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#if defined(_WIN32)
#include <winsock2.h> // WInsock2 API
#include <ws2tcpip.h> // getaddrinfo, inet_ntop (ipv6)
#include <mswsock.h> // AcceptEx, ConnectEx
#include <windows.h> // Windows API
#include <iphlpapi.h> // GetAdaptersInfo
#else defined (__linux__)
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef int SOCKET;
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#define closesocket(s) close(s)
#endif

#pragma comment(lib, "ws2_32.lib") // Winsock2 API
#pragma comment(lib, "winmm.lib") // timeGetTime
#pragma comment(lib, "iphlpapi.lib") // GetAdaptersInfo
#pragma comment(lib, "mswsock.lib") // AcceptEx, ConnectEx (Windows 7 SDK 이상에서 자동 링크, 그 이하 버전은 수동 링크 필요)

#include "define.h"

#pragma warning(disable: 4200) // zero-sized array in struct/union
#pragma warning(disable: 4251) // DLL-interface warning
#pragma warning(disable: 4275) // non DLL-interface class used as base for DLL-interface class
#pragma warning(disable: 4996) // deprecated function warning

import makga.lib.logger;