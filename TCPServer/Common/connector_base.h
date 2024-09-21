#pragma once

#include "common_type.h"
#include "config.h"
#include "net_actor.h"
#include "session_base.h"

class ConnectorBase : public NetActor
{
public:
	ConnectorBase();
	virtual ~ConnectorBase();

	void Initialize() override;
	void Finalize() override;

	bool Connect(const std::string& _ip, Port_t _port);

	bool OnAccepted(std::shared_ptr<SessionBase>, IOContext_t*) override;
	bool OnConnected(std::shared_ptr<SessionBase>, IOContext_t*) override;
	bool OnReceived(std::shared_ptr<SessionBase>, DWORD, IOContext_t*) override;
	bool OnSent(std::shared_ptr<SessionBase>, DWORD, IOContext_t*) override;
	bool OnClosed(std::shared_ptr<SessionBase>, IOContext_t*) override;

	std::shared_ptr<SessionBase> Session() const;
	void SetSession(std::shared_ptr<SessionBase> _session);

protected:
	std::shared_ptr<SessionBase> m_session;
};