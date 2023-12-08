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

    // IsSearchMode()
    // RunSearchMode()
    // IsWaitMode()
    // RunWaitMode()
    // IsMoveMode()
    // RunMode();
	
	m_root = core::ai::Builder()
        .composite<core::ai::Sequence>()
            .leaf<ActionNode>(m_actor->AI(), &ActorAI::HasDefaultAggro)
            .leaf<ActionNode>(m_actor->AI(), &ActorAI::IsMoveable)
            .composite<core::ai::Selector>()
                .composite<core::ai::Sequence>()  // Check if Actor is a proactive type
                    .leaf<ActionNode>(m_actor->AI(), &ActorAI::IsProactiveType)
                    .leaf<ActionNode>(m_actor->AI(), &ActorAI::HasTarget)
                    .leaf<ActionNode>(m_actor->AI(), &ActorAI::FindTarget)
                .end()
                .composite<core::ai::Sequence>()  // Check if Actor is an alert type
                    .leaf<ActionNode>(m_actor->AI(), &ActorAI::IsAlertType)
                    .leaf<ActionNode>(m_actor->AI(), &ActorAI::HasTarget)
                    .leaf<ActionNode>(m_actor->AI(), &ActorAI::WaitForAttackDelay)
                    .leaf<ActionNode>(m_actor->AI(), &ActorAI::Attack)
                .end()
                .composite<core::ai::Sequence>()  // Check if Actor is a non-first mover type
                    .leaf<ActionNode>(m_actor->AI(), &ActorAI::IsNonFirstMoverType)
                    .leaf<ActionNode>(m_actor->AI(), &ActorAI::MoveNextPos)
                .end()
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
