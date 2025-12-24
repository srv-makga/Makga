#include "pch.h"
#include "user_server.h"
#include "net_thread.h"
#include "job_thread.h"

import makga.lib.logger;

int main()
{
	makga::network::IPEndPoint ep("127.0.0.1", 12345);
	std::shared_ptr<makga::network::IocpCore> core = std::make_shared<makga::network::IocpCore>();
	std::shared_ptr<UserServer> user_server = std::make_shared<UserServer>(ep, core);
	if (nullptr == user_server)
	{
		makga::lib::MakgaLogger::Error() << "Failed to create UserServer.";
		return -1;
	}

	std::shared_ptr<NetThread> net_thread = std::make_shared<NetThread>(core);
	net_thread->Initialize();
	net_thread->CreateThread(std::thread::hardware_concurrency() * 2);

	std::shared_ptr<JobThread> job_thread = std::make_shared<JobThread>();
	job_thread->Initialize();
	job_thread->CreateThread(std::thread::hardware_concurrency());
	
	user_server->Initialize(5000, net_thread, job_thread);
	if (false == user_server->Start())
	{
		makga::lib::MakgaLogger::Error() << "Failed to start UserServer.";
		return -1;
	}

	return 0;
}