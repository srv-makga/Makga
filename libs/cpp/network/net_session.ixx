module;

export module network.session;
import network.service;

namespace makga::network {
export class NetSession
{
public:
	NetSession() = default;
	virtual ~NetSession() = default;

	virtual NetServiceType GetServiceType() const = 0;
};
} // namespace makga::network