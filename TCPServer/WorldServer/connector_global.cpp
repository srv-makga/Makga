#include "pch.h"
#include "connector_global.h"

ConnectorGlobal::ConnectorGlobal()
{
}

ConnectorGlobal::~ConnectorGlobal()
{
}

ThreadId_t ConnectorGlobal::ThreadId() const
{
	return 0;
}

bool ConnectorGlobal::OnConnected(SessionBase* _session, IOContext_t* _io_context)
{
	_session->OnConnect();

	LOG_INFO << "Game -> World Connected!";

	fbb fbb;
	fbb.Finish(fb::world::CreateSend_Reg(fbb, CONFIG.server_type));

	return _session->Send(fb::world::SendPid_Reg, fbb);
}