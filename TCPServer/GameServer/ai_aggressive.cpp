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
	// ������ ���̺��� �Է¹޾� ������� string���� Ű�� ������ ��带 ������
	// MemSequence

    // IsSearchMode()
    // RunSearchMode()
    // IsWaitMode()
    // RunWaitMode()
    // IsMoveMode()
    // RunMode();
	
	m_root = core::ai::Builder()
        .composite<core::ai::Selector>()
            .composite<core::ai::Sequence>()
                .leaf<ActionNode>(m_actor->AI(), &ActorAI::CheckSearchProcess)
                .leaf<ActionNode>(m_actor->AI(), &ActorAI::RunSearchProcess)
            .end()
            .composite<core::ai::Sequence>()
                .leaf<ActionNode>(m_actor->AI(), &ActorAI::CheckGuardProcess)
                .leaf<ActionNode>(m_actor->AI(), &ActorAI::RunGuardProcess)
            .end()
            .composite<core::ai::Sequence>()
                .leaf<ActionNode>(m_actor->AI(), &ActorAI::CheckFollowProcess)
                .leaf<ActionNode>(m_actor->AI(), &ActorAI::RunFollowProcess)
            .end()
            .composite<core::ai::Sequence>()
                .leaf<ActionNode>(m_actor->AI(), &ActorAI::CheckMoveProcess)
                .leaf<ActionNode>(m_actor->AI(), &ActorAI::RunMoveProcess)
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
