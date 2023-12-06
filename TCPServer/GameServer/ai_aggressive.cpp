#include "pch.h"
#include "ai_aggressive.h"
#include "actor.h"
#include "actor_ai.h"

AIAggressive::AIAggressive(Actor* _actor)
	: m_actor(_actor)
	, m_root(nullptr)
{
}

void AIAggressive::initialize()
{
	// 데이터 테이블에서 입력받아 만드려면 string으로 키를 가지고 노드를 만들자
	// MemSequence
	
	m_root = core::ai::Builder()
		.composite<core::ai::Sequence>()
			.leaf<ActionNode>(m_actor->AI(), &ActorAI::IsMoveable)
			.composite<core::ai::Selector>()
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::HasTarget)
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::FindTarget)
			.end()
		.end()
		.build();
}

void AIAggressive::terminate(Status s)
{
	switch (s)
	{

	}
}

AIAggressive::Status AIAggressive::update()
{
	m_root->tick();

	return Status::Success;
}
