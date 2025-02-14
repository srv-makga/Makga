#include "pch.h"
#include "actor_monster.h"
#include "actor_ai.h"
#include "terrain.h"

Monster::Monster()
{
}

Monster::~Monster()
{
}

//void Monster::Initialize()
//{
//	Actor::Initialize();
//
//	m_basic_table = nullptr;
//
//	if (nullptr != m_ai)
//	{
//		m_ai->Initialize();
//	}
//}
//
//void Monster::Finalize()
//{
//}
//
//bool Monster::SetTable(const ActorBasicTable* _table)
//{
//	if (nullptr == _table)
//	{
//		LOG_ERROR << "_table is nullptr.";
//		return false;
//	}
//
//	if (Type() != _table->actor_type)
//	{
//		LOG_ERROR << "Mismatch actor type. My:" << EnumNameeActorType(Type()) << " Table:" << EnumNameeActorType(_table->actor_type);
//		return false;
//	}
//
//	if (m_basic_table == _table)
//	{
//		return true;
//	}
//
//	m_basic_table = static_cast<const MonsterBasicTable*>(_table);
//
//	m_hp_mp_max = m_basic_table->hp_mp;
//	m_hp_mp_cur = m_hp_mp_max;
//
//
//	m_ai.reset();
//	m_ai = std::make_shared<ActorAI>(shared_from_this(), m_basic_table->ai_type);
//	if (nullptr == m_ai)
//	{
//		LOG_ERROR << "m_ai is nullptr";
//		return false;
//	}
//
//	m_ai->Initialize();
//
//	return true;
//}
//
//void Monster::OnUpdate()
//{
//	if (nullptr != m_ai)
//	{
//		m_ai->OnUpdate();
//	}
//}
//
//ActorUid_t Monster::ActorUid() const
//{
//	return m_uid;
//}
//
//std::shared_ptr<Actor> Monster::FindTarget()
//{
//	if (nullptr == m_basic_table)
//	{
//		return nullptr;
//	}
//
//	// 내 AI 타입이 선공이 아니면 실패
//	if (eAiType_Aggressive != m_basic_table->ai_type)
//	{
//		return nullptr;
//	}
//	
//	Terrain* terrain = CurTerrain();
//	if (nullptr == terrain)
//	{
//		return nullptr;
//	}
//
//	ActorList actor_list;
//	terrain->AroundList(Position(), MySight(), (int)FilterCharacter | FilterMonster, actor_list);
//
//	if (true == actor_list.empty())
//	{
//		return nullptr;
//	}
//
//	// @todo 이 중에서 제일 가까운 애를 찾아야 함
//
//	m_target = *actor_list.begin();
//	return m_target;
//}
//
//std::shared_ptr<Actor> Monster::Target() const
//{
//	return m_target;
//}
//
//Distance_t Monster::MySight() const
//{
//	return m_basic_table->sight;
//}
//
//Speed_t Monster::MoveBonusSpeed() const
//{
//	return Speed_t();
//}
//
//Speed_t Monster::MoveSpeed() const
//{
//	return m_basic_table->speed_move + MoveBonusSpeed();
//}
//
//fb::eAiType Monster::AIType() const
//{
//	return m_basic_table->ai_type;
//}
//
//Hp_t Monster::MaxBonusHp() const
//{
//	return Hp_t();
//}
//
//Mp_t Monster::MaxBonusMp() const
//{
//	return Mp_t();
//}
//
//Hp_t Monster::MaxHp() const
//{
//	return m_hp_mp_max.hp + MaxBonusHp();
//}
//
//Hp_t Monster::CurHp() const
//{
//	return m_hp_mp_cur.hp;
//}
//
//Mp_t Monster::MaxMp() const
//{
//	return m_hp_mp_max.mp + MaxBonusMp();
//}
//
//Mp_t Monster::CurMp() const
//{
//	return m_hp_mp_cur.mp;
//}
//
//Coord_t Monster::X() const
//{
//	return m_position.x;
//}
//
//Coord_t Monster::Y() const
//{
//	return m_position.y;
//}
//
//Coord_t Monster::Z() const
//{
//	return m_position.z;
//}
//
//bool Monster::IsDead() const
//{
//	return 0 >= CurHp();
//}