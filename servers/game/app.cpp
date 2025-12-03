import network;

#include <memory>

int main()
{
	std::shared_ptr<makga::network::NetService> service = makga::network::CreateNetService(makga::network::NetServiceType::Iocp);
	if (nullptr == service)
	{
		return 0;
	}

	return 0;
}