module;

export module makga.network.service;

namespace makga::network {
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
	NetService(NetServiceType type);
	virtual ~NetService();

	virtual bool Start();
	virtual bool Stop() = 0;

	NetServiceType GetServiceType() const;

protected:
	virtual bool StartUp() = 0;
	virtual bool StartUpEnd() = 0;

protected:
	NetServiceType service_type_;
};
} // namespace makga::network