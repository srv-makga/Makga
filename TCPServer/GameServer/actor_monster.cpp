#include "pch.h"
#include "actor_monster.h"
#include "actor_ai.h"

Monster::Monster()
{
}

Monster::~Monster()
{
}

void Monster::Initialize()
{
	m_basic_table = nullptr;
}

void Monster::Finallize()
{
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
		LOG_ERROR << "Mismatch ActorBasicTable. My:" << EnumNameeActorType(Type()) << " Table:" << EnumNameeActorType(_table->actor_type);
		return false;
	}

	m_basic_table = _table;


	switch (strhash(m_basic_table->ai_type.c_str()))
	{
	case strhash("Aggressive"):
		m_ai = new AIAggressive(this);
		break;
	}

	m_ai->initialize();

	return true;
}

void Monster::OnUpdate()
{
	m_ai->update();
}
