#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#if defined(_WIN32)
# include <winsock2.h>
# include <ws2tcpip.h>
# include <windows.h>
# include <mswsock.h>
//# pragma comment(lib, "ws2.lib")
# pragma comment(lib, "mswsock.lib")
#else
# include <sys/ioctl.h>
# include <poll.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <sys/uio.h>
# include <sys/un.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <net/if.h>
# include <limits.h>
#endif

#if defined(_WIN32)

inline const int socket_error_retval = SOCKET_ERROR;
inline const int max_addr_v4_str_len = 256;
inline const int max_addr_v6_str_len = 256;

using Socket_t = SOCKET;
using SocketAddr_t = sockaddr;

using InAddr4_t = in_addr;
using InMreq4_t = ip_mreq;
using SockAddrIn4_t = sockaddr_in;

using InAddr6_t = in6_addr;
using InMreq6_t = ipv6_mreq;
using SockAddrIn6_t = sockaddr_in6;

#else // linux

using Socket_t = int;

#define INVALID_SOCKET  (Socket_t)(~0)
#define SOCKET_ERROR    -1

#endif




# if defined(_WIN32) || defined(_WIN64)
# define OS_DEF(c) OS_DEF_##c
# define OS_DEF_AF_UNSPEC AF_UNSPEC
# define OS_DEF_AF_INET AF_INET
# define OS_DEF_AF_INET6 AF_INET6
# define OS_DEF_SOCK_STREAM SOCK_STREAM
# define OS_DEF_SOCK_DGRAM SOCK_DGRAM
# define OS_DEF_SOCK_RAW SOCK_RAW
# define OS_DEF_SOCK_SEQPACKET SOCK_SEQPACKET
# define OS_DEF_IPPROTO_IP IPPROTO_IP
# define OS_DEF_IPPROTO_IPV6 IPPROTO_IPV6
# define OS_DEF_IPPROTO_TCP IPPROTO_TCP
# define OS_DEF_IPPROTO_UDP IPPROTO_UDP
# define OS_DEF_IPPROTO_ICMP IPPROTO_ICMP
# define OS_DEF_IPPROTO_ICMPV6 IPPROTO_ICMPV6
# define OS_DEF_FIONBIO FIONBIO
# define OS_DEF_FIONREAD FIONREAD
# define OS_DEF_INADDR_ANY INADDR_ANY
# define OS_DEF_MSG_OOB MSG_OOB
# define OS_DEF_MSG_PEEK MSG_PEEK
# define OS_DEF_MSG_DONTROUTE MSG_DONTROUTE
# define OS_DEF_MSG_EOR 0 // Not supported on Windows.
# define OS_DEF_SHUT_RD SD_RECEIVE
# define OS_DEF_SHUT_WR SD_SEND
# define OS_DEF_SHUT_RDWR SD_BOTH
# define OS_DEF_SOMAXCONN SOMAXCONN
# define OS_DEF_SOL_SOCKET SOL_SOCKET
# define OS_DEF_SO_BROADCAST SO_BROADCAST
# define OS_DEF_SO_DEBUG SO_DEBUG
# define OS_DEF_SO_DONTROUTE SO_DONTROUTE
# define OS_DEF_SO_KEEPALIVE SO_KEEPALIVE
# define OS_DEF_SO_LINGER SO_LINGER
# define OS_DEF_SO_OOBINLINE SO_OOBINLINE
# define OS_DEF_SO_SNDBUF SO_SNDBUF
# define OS_DEF_SO_RCVBUF SO_RCVBUF
# define OS_DEF_SO_SNDLOWAT SO_SNDLOWAT
# define OS_DEF_SO_RCVLOWAT SO_RCVLOWAT
# define OS_DEF_SO_REUSEADDR SO_REUSEADDR
# define OS_DEF_TCP_NODELAY TCP_NODELAY
# define OS_DEF_IP_MULTICAST_IF IP_MULTICAST_IF
# define OS_DEF_IP_MULTICAST_TTL IP_MULTICAST_TTL
# define OS_DEF_IP_MULTICAST_LOOP IP_MULTICAST_LOOP
# define OS_DEF_IP_ADD_MEMBERSHIP IP_ADD_MEMBERSHIP
# define OS_DEF_IP_DROP_MEMBERSHIP IP_DROP_MEMBERSHIP
# define OS_DEF_IP_TTL IP_TTL
# define OS_DEF_IPV6_UNICAST_HOPS IPV6_UNICAST_HOPS
# define OS_DEF_IPV6_MULTICAST_IF IPV6_MULTICAST_IF
# define OS_DEF_IPV6_MULTICAST_HOPS IPV6_MULTICAST_HOPS
# define OS_DEF_IPV6_MULTICAST_LOOP IPV6_MULTICAST_LOOP
# define OS_DEF_IPV6_JOIN_GROUP IPV6_JOIN_GROUP
# define OS_DEF_IPV6_LEAVE_GROUP IPV6_LEAVE_GROUP
# define OS_DEF_AI_CANONNAME AI_CANONNAME
# define OS_DEF_AI_PASSIVE AI_PASSIVE
# define OS_DEF_AI_NUMERICHOST AI_NUMERICHOST
# else // linux
# endif



#include "throw_exception.hpp"


using IOService_t = SOCKET;

enum RioConfig
{
	CK_RIO = 0xC0DE,

	SESSION_BUFFER_SIZE = 65536,

	MAX_RECV_RQ_SIZE = 32,
	MAX_SEND_RQ_SIZE = 128,

	MAX_RIO_RESULT = 256,
	MAX_SEND_RQ_SIZE_PER_SOCKET = 32,
	MAX_RECV_RQ_SIZE_PER_SOCKET = 4,
	MAX_CLIENT = 10000,
	MAX_CQ_SIZE = (MAX_SEND_RQ_SIZE_PER_SOCKET + MAX_RECV_RQ_SIZE_PER_SOCKET) * MAX_CLIENT,
	RECV_BUFF_COUNT = 1,
	SEND_BUFF_COUNT = 1,

	SEND_LIMIT = 400,
	COMMIT_TIME = 20,
	MAX_POST_CNT = 128,
};