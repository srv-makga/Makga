module;

export module makga.network.service;

export namespace makga::network {
export enum class NetServiceType
{
	None,
	IocpServer,
	IocpClient,
	RioServer,
	RioClient,
	Iouring,
};

export class NetService
{
public:
	NetService(NetServiceType type)
		: service_type_(type) {}

	virtual ~NetService() = default;

	virtual bool Start()
	{
		if (false == StartUp())
		{
			return false;
		}

		if (false == StartUpEnd())
		{
			return false;
		}

		return true;
	}

	virtual bool Stop() = 0;

	NetServiceType GetServiceType() const { return service_type_; }

protected:
	virtual bool StartUp() = 0;
	virtual bool StartUpEnd() = 0;

protected:
	NetServiceType service_type_;
};
} // namespace makga::network