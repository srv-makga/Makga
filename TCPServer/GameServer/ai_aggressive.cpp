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
	// ������ ���̺��� �Է¹޾� ������� string���� Ű�� ������ ��带 ������
	// MemSequence
	m_root = core::ai::Builder()
		.composite<core::ai::Sequence>()
			.leaf<ActionNode>(m_actor, Actor::IsMovable())
	//m_root = core::ai::Builder()
	//	.composite<core::ai::Sequence>()
	//		.leaf<IsMoveAbleNode>(m_actor)
	//		.composite<core::ai::Selector>()
	//			.leaf<HasTargetNode>(m_actor)
	//			.leaf<FindTargetNode>(m_actor)
	//		.end()
	//	.end()
	//	.build();
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
