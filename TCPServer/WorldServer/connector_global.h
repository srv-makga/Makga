#pragma once

#include "../Common/connector_base.h"
#include "../Common/session_base.h"

class ConnectorGlobal : public ConnectorBase, public JobOwner
{
public:
	ConnectorGlobal();
	~ConnectorGlobal();

	ThreadId_t ThreadId() const override;
	bool OnConnected(SessionBase* _session, IOContext_t* _io_context) override;
};