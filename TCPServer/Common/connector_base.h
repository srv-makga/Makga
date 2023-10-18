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

	bool OnAccepted(SessionBase*, IOContext_t*) override;
	bool OnConnected(SessionBase*, IOContext_t*) override;
	bool OnReceived(SessionBase*, DWORD, IOContext_t*) override;
	bool OnSent(SessionBase*, DWORD, IOContext_t*) override;
	bool OnClosed(SessionBase*, IOContext_t*) override;

	SessionBase* Session() const;
	void SetSession(SessionBase* _session);

protected:
	SessionBase* m_session;
};