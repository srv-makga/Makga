#include "pch.h"
#include "user_server.h"
#include "net_thread.h"
#include "job_thread.h"

int main()
{
	makga::network::IPEndPoint ep("127.0.0.1", 12345);
	std::shared_ptr<makga::network::IocpCore> core = std::make_shared<makga::network::IocpCore>();
	std::shared_ptr<NetThread> net_thread = std::make_shared<NetThread>(core);
	std::shared_ptr<JobThread> job_thread = std::make_shared<JobThread>();

	std::shared_ptr<UserServer> user_server = std::make_shared<UserServer>(ep, core);
	user_server->Initialize(5000, net_thread, job_thread);
	user_server->Start();

	return 0;
}