module;

#include <memory>
#include <vector>
#include <functional>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif

export module makga.network.udp.socket;

export import makga.network.iocp.core;
export import makga.network.iocp.event;
export import makga.network.iocp.event;
export import makga.network.endpoint;

namespace makga::network {
export class UdpSocket : public IocpObject
{
public:
    UdpSocket(std::shared_ptr<IocpCore> iocp_core);
    virtual ~UdpSocket();

    bool Open(unsigned short port);
    void Close();

    bool PostRecv(); // post an overlapped WSARecvFrom
    bool SendTo(const IPEndPoint& remote, const char* data, int len);

    void SetOnMessage(std::function<void(const IPEndPoint&, const char*, int)> cb);

    // IocpObject
    void Dispatch(IocpEvent* iocp_event, int bytes_transferred = 0) override;
    HANDLE GetHandle() const override;

private:
    std::shared_ptr<IocpCore> iocp_core_;
    SOCKET socket_;
    std::function<void(const IPEndPoint&, const char*, int)> on_message_cb_;
};
} // namespace makga::network
