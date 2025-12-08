module makga.network.iocp.service;

namespace makga::network {
IocpService::IocpService()
	: NetService(NetServiceType::Iocp)
{
}

IocpService::~IocpService()
{

}

bool IocpService::Initialize()
{
	return true;
}

void IocpService::Finalize()
{

}
}// namespace makga::network