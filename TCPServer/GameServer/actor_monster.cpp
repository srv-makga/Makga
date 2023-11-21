#include "pch.h"
#include "actor_monster.h"

Monster::Monster()
	: m_ai(this)
{
}

Monster::~Monster()
{
}

void Monster::Initialize()
{
}

void Monster::Finallize()
{
}

void Monster::OnUpdate()
{
	m_ai->update();
}
