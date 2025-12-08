module;

#include <memory>

export module makga.network;
export import makga.network.service;
export import makga.network.iocp.service;
export import makga.network.session;
export import makga.network.endpoint;

export namespace makga::network {
using NetService_t = std::shared_ptr<NetService>;

export NetService_t CreateNetService(NetServiceType type)
{
	switch (type)
	{
	case NetServiceType::Iocp:
		return std::make_shared<IocpService>();
	case NetServiceType::Rio:
		return nullptr;
	case NetServiceType::Iouring:
		return nullptr;
	}

	return nullptr;
}
} // namespace makga::network