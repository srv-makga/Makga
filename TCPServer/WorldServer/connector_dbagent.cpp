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
	LOG_INFO << "DBAgent Connected!";

	_session->OnConnect();

	fbb fbb;
	fbb.Finish(fb::dbagent::CreateSend_Reg(fbb, CONFIG.server_type));
	return _session->Send(fb::dbagent::SendPid_Reg, fbb);
}