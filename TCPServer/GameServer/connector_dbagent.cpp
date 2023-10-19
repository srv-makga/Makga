#include "pch.h"
#include "connector_dbagent.h"

ConnectorDBAgent::ConnectorDBAgent()
{
}

ConnectorDBAgent::~ConnectorDBAgent()
{
}

bool ConnectorDBAgent::OnConnected(SessionBase* _session, IOContext_t* _io_context)
{
	_session->OnConnect();

	LOG_INFO << "DBAgent Connected!";

	fbb fbb;
	fbb.Finish(fb::world::CreateSend_Reg(fbb, CONFIG.server_type));

	return _session->Send(fb::world::SendPid_Reg, fbb);
}

ThreadId_t ConnectorDBAgent::ThreadId() const
{
    return 0;
}
