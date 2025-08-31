#include "pch.h"
#include "ai_aggressive.h"
#include "actor.h"
#include "actor_ai.h"

AIAggressive::AIAggressive(std::shared_ptr<Actor> _actor)
	: AI(_actor)
{
}

AIAggressive::~AIAggressive()
{
}

void AIAggressive::initialize()
{
	auto actor_ai = Owner()->GetActorAI();
	
	m_root = core::ai::Builder()
		.composite<core::ai::Selector>()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(actor_ai, &ActorAI::IsDead) // ���� ���¶��
				.decorator<core::ai::Succeeder>()
					.leaf<ActionNode>(actor_ai, &ActorAI::TryResurrection) // ��Ȱ�õ�
				.end()
			.end()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(actor_ai, &ActorAI::HasTarget) // Ÿ���� ������
				.composite<core::ai::Selector>()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(actor_ai ,&ActorAI::IsInsideAttackRange) // ���� ���� ������ Ȯ���ϰ�
						.leaf<ActionNode>(actor_ai ,&ActorAI::AttackTarget) // Ÿ�� ����
					.end()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(actor_ai ,&ActorAI::CheckFarSpawnPoistion) // ������������ �ָ� ���������� Ȯ���ϰ�
						.leaf<ActionNode>(actor_ai ,&ActorAI::MoveToTarget) // Ÿ�ٿ��� �̵�
					.end()
					.leaf<ActionNode>(actor_ai ,&ActorAI::ReturnRoutePosition) // ���� ��ǥ�� �̵�
				.end()
			.end()
			.composite<core::ai::Sequence>()
				.composite<core::ai::Selector>()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(actor_ai ,&ActorAI::FindTarget) // Ÿ�� ã��
						.leaf<ActionNode>(actor_ai ,&ActorAI::SetTarget) // Ÿ�� ����
					.end()
					.leaf<ActionNode>(actor_ai ,&ActorAI::Idle) // ��Ȳ ���·� ����
				.end()
			.end()
		.end()
		.build();
}

AIAggressive::Status AIAggressive::update()
{
	if (nullptr == m_root)
	{
		return Status::Failure;
	}

	return m_root->tick();
}
