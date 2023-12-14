#include "pch.h"
#include "ai_non_aggressive.h"
#include "actor.h"
#include "actor_ai.h"

AINonAggressive::AINonAggressive(Actor* _actor)
	: m_actor(_actor)
{
}

AINonAggressive::~AINonAggressive()
{
}

void AINonAggressive::initialize()
{
	// MemSequence
	m_root = core::ai::Builder()
		.composite<core::ai::Selector>()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::CheckResurrection) // ��Ȱ ������ ���¸�
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::Resurrection) // ��Ȱ�ϰ� ���� ������Ʈ ��ٸ�
			.end()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::HasTarget) // Ÿ���� ������
				.composite<core::ai::Selector>()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::IsInsideAttackRange) // ���� ������ �������� Ȯ�� ��
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::AttackTarget) // ������ ����
					.end()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::CheckFarSpawnPoistion) // ���� �������κ��� �ʹ� �ָ� �Դ���
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::MoveToTarget) // Ÿ�������� ��� �������� �Ѵ�
					.end()
					.leaf<ActionNode>(m_actor->AI(), &ActorAI::ReturnRoutePosition) // ������ route ��ġ�� ��ȯ
				.end()
			.end()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::NextRoute) // ���� route ��ġ�� �̵�
			.end()
		.end()
		.build();
}

void AINonAggressive::terminate(Status s)
{
}

AINonAggressive::Status AINonAggressive::update()
{
	m_root->update();
}
