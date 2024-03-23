#include "pch.h"
#include "actor_monster.h"
#include "actor_ai.h"
#include "terrain.h"

Monster::Monster(ActorUid_t _uid)
	: Actor(_uid)
{
}

Monster::~Monster()
{
	delete m_ai;
}

void Monster::Initialize()
{
	m_basic_table = nullptr;

	if (nullptr != m_ai)
	{
		m_ai->Initialize();
	}

	Actor::Initialize();
}

void Monster::Finallize()
{
	Actor::Finallize();
}

bool Monster::SetTable(ActorBasicTable* _table)
{
	if (nullptr == _table)
	{
		LOG_ERROR << "_table is nullptr.";
		return false;
	}

	if (Type() != _table->actor_type)
	{
		LOG_ERROR << "Mismatch actor type. My:" << EnumNameeActorType(Type()) << " Table:" << EnumNameeActorType(_table->actor_type);
		return false;
	}

	if (m_basic_table == _table)
	{
		return true;
	}

	m_basic_table = _table;

	delete m_ai;

	m_ai = new ActorAI(this, m_basic_table->ai_type);
	if (nullptr == m_ai)
	{
		LOG_ERROR << "m_ai is nullptr";
		return false;
	}

	m_ai->Initialize();

	return true;
}

void Monster::OnUpdate()
{
	if (nullptr != m_ai)
	{
		m_ai->OnUpdate();
	}
}

Actor* Monster::FindTarget()
{
	if (nullptr == m_basic_table)
	{
		return nullptr;
	}

	// 내 AI 타입이 선공이 아니면 실패
	if (eAiType_Aggressive != m_basic_table->ai_type)
	{
		return nullptr;
	}
	
	Terrain* terrain = CurTerrain();
	if (nullptr == terrain)
	{
		return nullptr;
	}

	ActorList actor_list;
	terrain->AroundList(Position(), MySight(), (int)FilterCharacter | FilterMonster, actor_list);

	if (true == actor_list.empty())
	{
		return nullptr;
	}

}
