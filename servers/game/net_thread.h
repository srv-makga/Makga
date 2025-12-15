#pragma once

import makga.network.iocp.core;
//import makga.network.iocp.event;
//import makga.network.iocp.object;
//import makga.network.iocp.session;
import makga.network.nethandler;
import makga.lib.thread.group;

class NetThread : public makga::network::NetHandler
{
public:
	NetThread(std::shared_ptr<makga::network::IocpCore> core);
	virtual ~NetThread();

	bool Initialize();
	void Finalize();
	void Run();

public: // NetHandler
	bool CreateThread(std::size_t thread_count = 1) override;
	bool Start() override;
	void Stop() override;

protected:
	std::shared_ptr<makga::network::IocpCore> core_;
	makga::lib::ThreadGroup thread_group_;
	std::size_t thread_count_;
};