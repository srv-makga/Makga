#include "pch.h"
#include "ai_non_aggressive.h"
#include "actor.h"
#include "actor_ai.h"

AINonAggressive::AINonAggressive(std::shared_ptr<Actor> _actor)
	: AI(_actor)
{
}

AINonAggressive::~AINonAggressive()
{
}

void AINonAggressive::initialize()
{
	auto actor_ai = Owner()->GetActorAI();

	m_root = core::ai::Builder()
		.composite<core::ai::Selector>()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(actor_ai, &ActorAI::IsDead) // 죽은 상태라면
				.decorator<core::ai::Succeeder>()
					.leaf<ActionNode>(actor_ai, &ActorAI::TryResurrection) // 부활 시도
				.end()
			.end()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(actor_ai, &ActorAI::HasTarget) // 현재 타겟이 있으면
				.composite<core::ai::Selector>()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(actor_ai, &ActorAI::IsInsideAttackRange) // 공격 범위 내인지 판단
						.leaf<ActionNode>(actor_ai, &ActorAI::AttackTarget) // 공격 진행
					.end()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(actor_ai, &ActorAI::CheckFarSpawnPoistion) // 혹시 스폰지역에서 멀리 떨어졌는지
						.leaf<ActionNode>(actor_ai, &ActorAI::MoveToTarget) // 타겟에게 이동
					.end()
					.leaf<ActionNode>(actor_ai, &ActorAI::ReturnRoutePosition) // 다음 좌표로 이동
				.end()
			.end()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(actor_ai, &ActorAI::Idle) // 순회 상태로 변경
			.end()
		.end()
		.build();
}

AINonAggressive::Status AINonAggressive::update()
{
	m_root->update();

	return AINonAggressive::Status::Success;
}
