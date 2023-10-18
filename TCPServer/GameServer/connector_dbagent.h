#pragma once

#include "../Common/connector_base.h"
#include "../Common/session_base.h"

class ConnectorDBAgent : public ConnectorBase, public JobOwner
{
public:
	ConnectorDBAgent();
	~ConnectorDBAgent();

	bool OnConnected(SessionBase* _session, IOContext_t* _io_context);

	ThreadId_t ThreadId() const override;
};
