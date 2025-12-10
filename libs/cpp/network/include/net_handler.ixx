module;

#include <memory>

export module makga.network.nethandler;

import makga.network.iocp.core;

export namespace makga::network {
export class NetHandler
{
public:
	NetHandler() = default;
	virtual ~NetHandler() = default;

	virtual bool CreateThread(std::size_t thread_count = 1) = 0;
	virtual bool Start() = 0;
	virtual void Stop() = 0;
};
} // namespace makga::network