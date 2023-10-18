#pragma once

#include "../Common/connector_base.h"
#include "../Common/session_base.h"

class ConnectorCommunity : public ConnectorBase, public JobOwner
{
public:
	ConnectorCommunity();
	~ConnectorCommunity();

	ThreadId_t ThreadId() const override;
};