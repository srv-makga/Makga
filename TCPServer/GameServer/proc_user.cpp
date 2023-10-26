#include "pch.h"
#include "user.h"
#include "session_dbagent.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::server::SendPid_##pid, &User::On##pid);

core::Dispatcher<User::Pid_t, User::Function_t> User::s_dispatcher;

bool User::InitDispatcher()
{
	REG_DISPATCHER(LoginSecurity);
	REG_DISPATCHER(CharacterCreate);
	REG_DISPATCHER(CharacterDelete);
	REG_DISPATCHER(CharacterSelect);
	REG_DISPATCHER(CharacterLogout);

	return true;
}

bool User::OnLoginSecurity(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << fb::EnumNameeResult(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_LoginSecurity);
	if (nullptr == recv_data)
	{
		return false;
	}

	CREATE_FBB(fbb);
	fbb.Finish(fb::dbagent::CreateSend_LoginSecurity(fbb,
		fb::dbagent::CreateHeader(fbb, UserUid()),
		recv_data->security_number()
	));

	return SESSION_DBAGENT.Send(fb::dbagent::SendPid_LoginSecurity, fbb);
}

bool User::OnCharacterCreate(NetPacket* _packet)
{
	return false;
}

bool User::OnCharacterDelete(NetPacket* _packet)
{
	return false;
}

bool User::OnCharacterSelect(NetPacket* _packet)
{
	return false;
}

bool User::OnCharacterLogout(NetPacket* _packet)
{
	return false;
}