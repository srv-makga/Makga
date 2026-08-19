module;

#include <memory>

export module makga.network;
export import makga.network.service;
export import makga.network.iocp.service;
export import makga.network.session;
export import makga.network.endpoint;

namespace makga::network {
} // namespace makga::network
export import makga.network.rudp.session;
export import makga.network.realtime.transport;
export import makga.network.iouring.core;
export import makga.network.iouring.service;
export import makga.network.iouring.session;
export import makga.network.iouring.acceptor;
