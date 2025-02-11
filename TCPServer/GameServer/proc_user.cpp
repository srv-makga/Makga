#include "pch.h"
#include "user.h"
#include "../Common/utility.h"
#include "server_game.h"
#include "data_manager.h"
#include "inventory_user.h"
#include "actor_character.h"
#include "actor_manager.h"

//#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::server::SendPid_##pid, &User::On##pid);
#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::server::SendPid_##pid, [](User* _user, std::shared_ptr<Packet> packet) -> bool { return _user->On##pid(packet); });

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

bool User::OnChatting(std::shared_ptr<Packet> _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_Chatting);

	return true;
}

bool User::OnLoginSecurity(std::shared_ptr<Packet> _packet)
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
	return SERVER.m_session_dbagent->Send(fb::dbagent::SendPid_LoginSecurity, fbb);
}

bool User::OnCharacterCreate(std::shared_ptr<Packet> _packet)
{
	return false;
}

bool User::OnCharacterDelete(std::shared_ptr<Packet> _packet)
{
	return false;
}

bool User::OnCharacterSelect(std::shared_ptr<Packet> _packet)
{
	return false;
}

bool User::OnCharacterLogout(std::shared_ptr<Packet> _packet)
{
	return false;
}

bool User::OnCharacterMove(std::shared_ptr<Packet> _packet)
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

		fb::PositionT pos;
		recv_data->pos()->UnPackTo(&pos);

		result = ActiveCharacter()->DoMove(pos);
	} while (false);

	LOG_ERROR_IF(eResult_Success != result) << LOG_RESULT(result) << LOG_USER_UID(UserUid());

	return true;
}

bool User::OnCharacterAngle(std::shared_ptr<Packet> _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_CharacterAngle);

	return true;
}