#include "pch.h"
#include "user.h"
#include "data_manager.h"
#include "session_dbagent.h"
#include "../Common/utility.h"
#include "inventory_account.h"
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
	return false;
}

bool User::OnCharacterAngle(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_CharacterAngle);

	GetCharacter()->SetAngle(recv_data->angle());
}

bool User::OnCharacterResurrection(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_CharacterResurrection);
	eResult result = eResult_Success;

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
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_ActorInteractionStart);
	Result_t result = eResult_Success;

	do
	{
		auto target = ACTOR_MANAGER.Find(recv_data->target_actor_id());
		if (nullptr == target)
		{
			result = eResult_ActorNotFound;
			break;
		}

		m_interaction_id = recv_data->target_actor_id();
		m_interaction_expire = UTIL.CurrentTime() + DATA_MANAGER.SystemValue().interraction_wait_time;
		target->set_interaction_id(actor_id());
	} while (false);

	LOG_ERROR_IF(eResult_Success != result) << LOG_RESULT(result) << "actor_id:" << recv_data->target_actor_id();

	return true;
}

bool User::OnActorInteractionEnd(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_ActorInteractionEnd);
	Result_t result = eResult_Success;

	do
	{
		if (m_interaction_id != recv_data->target_actor_id())
		{
			result = 상호작용을 한 상대방이 아니다;
			break;
		}

		if (m_interaction_expire)

		Actor* target = ACTOR_MANAGER.Find(recv_data->target_actor_id());
		if (nullptr == target)
		{
			result = 액터를 찾을 수 없다;
			break;
		}

		if (actor_id() != target->interaction_id())
		{
			result = 상호작용을 한 상대방이 아니다;
			break;
		}

		m_interaction_id = 0;
		m_interaction_expire = 0;
	} while (false);
	
	LOG_ERROR_IF(eResult_Success != result) << LOG_RESULT(result) << "actor_id:" << recv_data->target_actor_id();

	return true;
}

bool User::OnActorInteractionCancel(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_ActorInteractionEnd);
	Result_t result = eResult_Success;

	do
	{
		m_interaction_id = 0;
		m_interaction_expire = 0;
	} while (false);

	return true;
}

bool User::OnItemDestroy(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_ItemDestroy);
	Result_t result = eResult_Success;

	do
	{
		auto item_object = FindItemObject(recv_data->uid());
		if (nullptr == item_object)
		{
			result = eResult_ItemNotFound;
			break;
		}

		result = Inventory(*item_object->Property())->SubItem(recv_data->uid(), recv_data->stack());
	} while (false);

	LOG_ERROR_IF(eResult_Success != result) << LOG_RESULT(result) << " item_uid:" << recv_data->uid();

	CREATE_FBB(fbb);
	fbb.Finish(fb::server::CreateRecv_ItemDestroy(fbb,
		result,
		recv_data->uid(),
		recv_data->stack()
	));
	return Send(fb::server::RecvPid_ItemDestroy, fbb);
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
	if (nullptr == _packet)
	{
		LOG_ERROR << LOG_USER(this) << LOG_RESULT(eResult_InvalidParameter);
		return false;
	}

	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::server::Send_ItemSkinChange);
	eResult result = eResult_Success;

	do
	{
		auto item_object = FindItemObject(recv_data->uid());
		if (nullptr == item_object)
		{
			result = eResult_ItemNotFound;
			break;
		}

		if (item_object->SkinIndex() == recv_data->skin_item_idx())
		{
			result = eResult_ItemSameSkinIndex;
			break;
		}

		item_object->SetSkinIndex(recv_data->skin_item_idx());
		item_object->Reflection(ReflectType::UpdateSkin, m_inventory);

	} while (false);

	LOG_ERROR_IF(eResult_Success != result) << LOG_USER(this) << LOG_RESULT(result) << " item_uid:" << recv_data->uid();

	CREATE_FBB(fbb);
	fbb.Finish(fb::server::CreateRecv_ItemSkinChange(fbb,
		result,
		recv_data->uid(),
		recv_data->skin_item_idx()
	));
	Send(fb::server::RecvPid_ItemSkinChange, fbb);

	return true;
}