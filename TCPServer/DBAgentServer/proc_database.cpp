#include "pch.h"
#include "database_client.h"
#include "config_dbagent.h"
#include "session_server.h"

bool DatabaseClient::InitDispatcher()
{
	ADD_RECV_DBA(Reg);
	ADD_RECV_DBA(LoginAuth);

	return true;
}

bool DatabaseClient::OnRecvReg(SessionBase* _session, NetPacket* _packet)
{
	// server
	return true;
}

bool DatabaseClient::OnRecvLoginAuth(SessionBase* _session, NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::dbagent::Recv_LoginAuth);

	eResult result = eResult_Success;

	// auto iter = m_stmts.find(dbagent::SendPid_LoginAuth);
	auto iter = m_stmts.find(static_cast<dbagent::SendPid>(_packet->Id()));
	if (m_stmts.end() == iter)
	{
		return false;
	}

	try
	{
		do
		{
			std::string account = recv_data->account()->c_str();
			std::string auth_key = recv_data->auth_key()->c_str();

			iter->second->Bind(1, account);
			iter->second->Bind(2, auth_key);
			iter->second->Execute();

			if (false == iter->second->Fetch())
			{
				break;
			}

			int val = 0;

			int seq = 0;
			iter->second->Get(++seq, val);

			// @todo DB ´Ù³à¿Í¾ßÁö
		} while (false);
	}
	catch (const std::exception& e)
	{
		LOG_ERROR << e.what();
	}

	CREATE_FBB(fbb);
	fbb.Finish(fb::dbagent::CreateRecv_LoginAuth(fbb,
		fb::dbagent::CreateHeader(fbb, recv_data->header()->uid())
		, fbb.CreateString(recv_data->account())
		, fbb.CreateString(recv_data->auth_key())
		, result
	));

	return _session->Send(dbagent::RecvPid_LoginAuth, fbb);
}
