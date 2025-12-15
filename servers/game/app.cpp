#include "pch.h"
#include "../common/iocp_server.h"
#include "net_thread.h"
#include "job_thread.h"

int main()
{
	makga::network::IPEndPoint ep("127.0.0.1", 12345);
	std::shared_ptr<makga::network::IocpCore> core = std::make_shared<makga::network::IocpCore>();
	std::shared_ptr<NetThread> net_thread = std::make:shared<NetThread>(core);
	std::shared_ptr<JobThread> job_thread = std::make:shared<JobThread>();

	std::shared_ptr<IocpServer> app = std::make_shared<IocpServer>(ep, core);
	app->Initialize(5000, net_thread, job_thread);
	app->Start();

	return 0;
}