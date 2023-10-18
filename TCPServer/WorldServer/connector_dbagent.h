#pragma once

#include "../Common/connector_base.h"
#include "../Common/session_base.h"

class ConnectorDBAgent : public ConnectorBase
{
public:
	ConnectorDBAgent();
	virtual ~ConnectorDBAgent();

	bool OnConnected(SessionBase* _session, IOContext_t* _io_context) override;
};