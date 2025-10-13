#include "pch.h"
#include "user.h"
#include "../Common/utility.h"
#include "server_game.h"
#include "data_manager.h"
#include "inventory_user.h"
#include "actor_character.h"
#include "actor_manager.h"

//#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::server::SendPid_##pid, &User::On##pid);
#define REG_DISPATCHER(type, pid)	s_dispatcher[type].Add(fb::server::SendPid_##pid, [](User* _user, std::shared_ptr<Packet> packet) -> bool { return _user->On##pid(packet); });

void RegistDispatcher(CommandType _type, fb::server::SendPid _pid)
{
}

bool User::InitDispatcher()
{
	REG_DISPATCHER(CommandType::User, Chatting);
	REG_DISPATCHER(CommandType::User, LoginSecurity);
	REG_DISPATCHER(CommandType::User, CharacterCreate);
	REG_DISPATCHER(CommandType::User, CharacterDelete);
	REG_DISPATCHER(CommandType::User, CharacterSelect);
	REG_DISPATCHER(CommandType::User, CharacterLogout);
	REG_DISPATCHER(CommandType::User, CharacterMove);
	REG_DISPATCHER(CommandType::User, CharacterAngle);

	return true;
}

bool User::ProcPacket(std::shared_ptr<NetPacket> _packet)
{
	fb::server::SendPid pid = static_cast<fb::server::SendPid>(_packet->GetId());

	LOG_INFO << "ProcPacket Start. Pid:" << LOG_USER_UID(UserUid()) << fb::server::EnumNameSendPid(pid);

	bool ret = s_dispatcher[CommandType::User].Exec(pid, this, _packet); // @todo 수정 필요
	//bool ret = Messenger::ProcPacket(CommandType::User, shared_from_this(), pid, _packet);

	LOG_INFO << "ProcPacket End. Pid:" << LOG_USER_UID(UserUid()) << fb::server::EnumNameSendPid(pid) << " result:" << ret ? "true" : "false";

	return ret;
}

bool User::OnChatting(std::shared_ptr<Packet> _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_Chatting);
	if (nullptr == recv_data)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

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
	GameServer::GetDBAgentServer()->Send(static_cast<PacketId_t>(fb::dbagent::SendPid_LoginSecurity), fbb);

	return true;
}

bool User::OnCharacterCreate(std::shared_ptr<Packet> _packet)
{
	do
	{
		if (nullptr == m_character)
		{
			m_character = std::make_shared<Character>(); // @todo 풀링
		}

		auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_CharacterCreate);

		GetWorld().data.FindCharacterBasicTable(recv_data->detail()->base()->idx());

		m_character->Initialize();
		m_character->SetActorTable();
		m_character->Setup(*recv_data->detail());
		m_character->SaveDB();

	} while (false);

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