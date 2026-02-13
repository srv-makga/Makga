module;

export module makga.network.acceptor;

import <iostream>;

namespace makga::network {
export class NetAcceptor
{
public:
	virtual ~NetAcceptor() = default;

	virtual bool Start() = 0;
	virtual void Stop() = 0;

	virtual bool CanAcceptSession() const = 0;
    virtual std::size_t GetConnectCount() const = 0;
    virtual std::size_t GetMaxConnectCount() const = 0;
};
} // namespace makga::network