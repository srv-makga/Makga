#include "pch.h"
#include "user.h"
#include "data_manager.h"
#include "session_dbagent.h"
#include "actor_character.h"
#include "actor_manager.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::server::SendPid_##pid, &User::On##pid);

core::Dispatcher<User::Pid_t, User::Function_t> User::s_dispatcher;

bool User::InitDispatcher()
{
	REG_DISPATCHER(LoginSecurity);
	REG_DISPATCHER(CharacterCreate);
	REG_DISPATCHER(CharacterDelete);
	REG_DISPATCHER(CharacterSelect);
	REG_DISPATCHER(CharacterLogout);
	REG_DISPATCHER(CharacterMove);
	REG_DISPATCHER(CharacterAngle);
	REG_DISPATCHER(CharacterResurrection);

	REG_DISPATCHER(ActorInteractionStart);
	REG_DISPATCHER(ActorInteractionEnd);
	REG_DISPATCHER(ActorInteractionCancel);

	REG_DISPATCHER(ItemDestroy);
	REG_DISPATCHER(ItemUse);
	REG_DISPATCHER(ItemMake);
	REG_DISPATCHER(ItemReinforce);
	REG_DISPATCHER(ItemDisassemble);
	REG_DISPATCHER(ItemEnchant);
	REG_DISPATCHER(ItemSkinChange);

	return true;
}

bool User::OnLoginSecurity(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(InvalidParameter);
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

bool User::OnCharacterMove(NetPacket* _packet)
{
	return false;
}

bool User::OnCharacterAngle(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_CharacterAngle);
	if (nullptr == recv_data)
	{
		return false;
	}

	GetCharacter()->SetAngle(recv_data->angle());
}

bool User::OnCharacterResurrection(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_CharacterResurrection);
	if (nullptr == recv_data)
	{
		return false;
	}

	eResult result = eResult::eResult_Success;

	do
	{
		if (0 < GetCharacter()->CurHp())
		{
			result = fb::eResult_CurrencyLackGold;
			break;
		}

		Gold_t need_gold = DATA_MANAGER.SystemValue().resurrection_need_currency;

	} while (false);

	return true;
}

bool User::OnActorInteractionStart(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_ActorInteractionStart);
	if (nullptr == recv_data)
	{
		return false;
	}

	auto target = ACTOR_MANAGER.Find(recv_data->target_actor_id());
	if (nullptr == target)
	{
		LOG_ERROR << LOG_RESULT(ActorNotFound) << "actor_id:" << recv_data->target_actor_id();
		return false;
	}
	
	m_interaction_id = recv_data->target_actor_id();
	m_interaction_expire = ::time(nullptr) + 30;

	return true;
}

bool User::OnActorInteractionEnd(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_ActorInteractionEnd);
	if (nullptr == recv_data)
	{
		return false;
	}

	m_interaction_id = 0;
	m_interaction_expire = 0;

	return true;
}

bool User::OnActorInteractionCancel(NetPacket* _packet)
{
	return false;
}

bool User::OnItemDestroy(NetPacket* _packet)
{
	return true;
}

bool User::OnItemUse(NetPacket* _packet)
{
	return false;
}

bool User::OnItemMake(NetPacket* _packet)
{
	return false;
}

bool User::OnItemReinforce(NetPacket* _packet)
{
	return false;
}

bool User::OnItemDisassemble(NetPacket* _packet)
{
	return false;
}

bool User::OnItemEnchant(NetPacket* _packet)
{
	return false;
}

bool User::OnItemSkinChange(NetPacket* _packet)
{
	return false;
}

Character* User::GetCharacter() const
{
	return m_character;
}

void User::SetCharacter(Character* _character)
{
	m_character = _character;
}
