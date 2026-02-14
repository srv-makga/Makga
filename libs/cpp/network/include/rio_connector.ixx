module;

#include <atomic>
#include <memory>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif

export module makga.network.rio.connector;

import makga.network.connector;
import makga.network.rio.service;
import makga.network.endpoint;
import makga.network.session;

namespace makga::network {

export class RioConnector : public NetConnector {
public:
  RioConnector(std::shared_ptr<RioService> service);
  virtual ~RioConnector();

  bool Start() override;
  void Stop() override;

  bool IsConnected() const override;

  bool Connect(const IPEndPoint &endpoint);

private:
  std::shared_ptr<RioService> service_;
  SOCKET socket_;
  std::atomic<bool> is_connected_;

  std::shared_ptr<NetSession> session_;
};

} // namespace makga::network
