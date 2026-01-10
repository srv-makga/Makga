#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#if defined(_WIN32)
#include <winsock2.h> // 소켓 생성, 연결, 전송들 WInsock2 API 기능 포함
#include <ws2tcpip.h> // getaddrinfo, inet_ntop등 주소 변환 함수 (ipv6등)
#include <mswsock.h> // AcceptEx, ConnectEx 등 고급 소켓 함수 포함
#include <windows.h> // 윈도우 전반적인 API 포함
#include <iphlpapi.h> // 윈도우 전반적인 API 포함
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

#pragma comment(lib, "ws2_32.lib") // 소켓 함수들의 실제 구현체
#pragma comment(lib, "winmm.lib") // timeGetTime
#pragma comment(lib, "iphlpapi.lib") // GetAdaptersInfo
#pragma comment(lib, "mswsock.lib")

#include "define.h"

#pragma warning(disable: 4200) // zero-sized array in struct/union
#pragma warning(disable: 4251) // DLL-interface warning
#pragma warning(disable: 4275) // non DLL-interface class used as base for DLL-interface class
#pragma warning(disable: 4996) // deprecated function warning

import makga.lib.logger;