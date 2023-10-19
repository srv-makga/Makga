#include "pch.h"
#include "database_client.h"
#include "config_dbagent.h"
#include "session_server.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::dbagent::SendPid_##pid, &DatabaseClient::On##pid);

bool DatabaseClient::InitDispatcher()
{
	REG_DISPATCHER(Reg);
	REG_DISPATCHER(LoginAuth);

	return true;
}

bool DatabaseClient::OnReg(SessionBase* _session, NetPacket* _packet)
{
	return true;
}

bool DatabaseClient::OnLoginAuth(SessionBase* _session, NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::dbagent::Recv_LoginAuth);

	eResult result = eResult_Success;

	auto iter = m_stmts.find(dbagent::SendPid_LoginAuth);
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

			// @todo DB 다녀와야지
		} while (false);
	}
	catch (const std::exception& e)
	{
		LOG_ERROR << e.what();
	}

	CREATE_FBB(fbb);
	fbb.Finish(fb::dbagent::CreateRecv_LoginAuth(fbb
		, fbb.CreateString(recv_data->account())
		, fbb.CreateString(recv_data->auth_key())
		, result
	));

	return _session->Send(dbagent::RecvPid_LoginAuth, fbb);
}
