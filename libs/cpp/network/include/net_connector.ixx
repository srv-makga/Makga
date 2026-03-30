module;

export module makga.network.connector;

namespace makga::network {
export class NetConnector
{
public:
	virtual ~NetConnector() = default;

	virtual bool Start() = 0;
	virtual void Stop() = 0;

	virtual bool IsConnected() const = 0;
};
} // namespace makga::network
