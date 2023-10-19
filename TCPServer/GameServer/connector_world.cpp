#include "pch.h"
#include "connector_world.h"

ConnectorWorld::ConnectorWorld()
{
}

ConnectorWorld::~ConnectorWorld()
{
}

bool ConnectorWorld::OnConnected(SessionBase* _session, IOContext_t* _io_context)
{
	_session->OnConnect();

	LOG_INFO << "Game -> World Connected!";

	fbb fbb;
	fbb.Finish(fb::world::CreateSend_Reg(fbb, CONFIG.server_type));

	return _session->Send(fb::world::SendPid_Reg, fbb);
}