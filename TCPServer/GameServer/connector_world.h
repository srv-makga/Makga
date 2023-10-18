#pragma once

#include "../Common/connector_base.h"
#include "../Common/session_base.h"

class ConnectorWorld : public ConnectorBase
{
public:
	ConnectorWorld();
	~ConnectorWorld();

	bool OnConnected(SessionBase* _session, IOContext_t* _io_context) override;
};