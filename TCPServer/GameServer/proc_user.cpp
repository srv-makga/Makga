#include "pch.h"
#include "user.h"
#include "data_manager.h"
#include "session_dbagent.h"
#include "../Common/utility.h"
#include "inventory_user.h"
#include "actor_character.h"
#include "actor_manager.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::server::SendPid_##pid, &User::On##pid);

core::Dispatcher<User::Pid_t, User::Function_t> User::s_dispatcher;

bool User::InitDispatcher()
{
	REG_DISPATCHER(Chatting);
	REG_DISPATCHER(LoginSecurity);
	REG_DISPATCHER(CharacterCreate);
	REG_DISPATCHER(CharacterDelete);
	REG_DISPATCHER(CharacterSelect);
	REG_DISPATCHER(CharacterLogout);
	REG_DISPATCHER(CharacterMove);
	REG_DISPATCHER(CharacterAngle);

	return true;
}

bool User::OnChatting(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_Chatting);

	return true;
}

bool User::OnLoginSecurity(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_LoginSecurity);

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

bool User::OnCharacterMove(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_CharacterMove);
	Result_t result = eResult_Success;

	do
	{
		if (nullptr == recv_data->pos())
		{
			result = eResult_InvalidParameter;
			break;
		}

		result = ActiveCharacter()->Move(recv_data->pos()->x(),
			recv_data->pos()->y(),
			recv_data->pos()->z(),
			recv_data->pos()->angle()
		);
	} while (false);

	LOG_ERROR_IF(eResult_Success != result) << LOG_RESULT(result);

	return true;
}

bool User::OnCharacterAngle(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_CharacterAngle);

	ActiveCharacter()->SetAngle(recv_data->angle());

	return true;
}