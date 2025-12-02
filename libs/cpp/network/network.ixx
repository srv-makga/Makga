module;

#include <memory>

export module network;
export import network.service;
export import network.service.iocp;
export import network.session;

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