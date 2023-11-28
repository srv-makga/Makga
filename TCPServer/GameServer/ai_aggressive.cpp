#include "pch.h"
#include "ai_aggressive.h"
#include "actor.h"

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
			.leaf<IsMoveAbleNode>(m_actor)
			.leaf<HasTargetNode>(m_actor)
			//.leaf<ActionNode<>>([this]()->core::ai::Node::Status { return m_actor->HasTarget() ? Status::Success : Status::Failure; })
			//.leaf<ActionNode<>>([this]()->core::ai::Node::Status { return m_actor->Target() ? Status::Success : Status::Failure; })
			//.leaf<ActionNode<>>([this]()->core::ai::Node::Status { return m_actor->Target()->IsDie() ? Status::Success : Status::Failure; })
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
