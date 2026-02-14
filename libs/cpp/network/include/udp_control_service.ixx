module;

#include <iostream>
#include <atomic>
#include <thread>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif

export module makga.network.udp.control;

import makga.network.socket.util;
import makga.network.udp.group;

namespace makga::network {
export class UdpControlService
{
public:
    UdpControlService(unsigned short tcp_port);
    ~UdpControlService();

    bool Start();
    void Stop();

private:
    void AcceptThread();
    void HandleClient(SOCKET client_socket);

private:
    unsigned short tcp_port_;
    std::atomic<bool> running_;
    std::thread accept_thread_;

    bool wsa_started_ = false;

    UdpGroup group_manager_;
};
} // namespace makga::network
