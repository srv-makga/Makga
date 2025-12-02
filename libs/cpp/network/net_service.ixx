module;

export module network.service;

export namespace makga::network {
enum class NetServiceType
{
	None,
	Iocp,
	Rio,
	Iouring,
};

class NetService
{
public:
	NetService(NetServiceType type)
		: service_type_(type) {}
	virtual ~NetService() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;

protected:
	NetServiceType service_type_;
};
} // namespace makga::network