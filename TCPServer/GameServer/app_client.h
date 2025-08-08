#pragma once

#include "game_header.h"
#include "app_config.h"
#include "../Common/iocp_client.h"
#include "pool.h"

class AppClient : public Client_t
{
public:
	AppClient(std::shared_ptr<core::network::IocpCore> _core, const core::network::IPEndPoint& _endpoint);
	virtual ~AppClient();

public: // Client_t
	bool Initialize() override;
	void Finalize() override;

	void Send(PacketId_t _pid, FB_BUILDER& _fbb);
};
