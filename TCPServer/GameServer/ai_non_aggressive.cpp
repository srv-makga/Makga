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
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::CheckResurrection) // 부활 가능한 상태면
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::Resurrection) // 부활하고 다음 업데이트 기다림
			.end()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::HasTarget) // 타겟이 있으면
				.composite<core::ai::Selector>()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::IsInsideAttackRange) // 공격 가능한 범위인지 확인 후
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::AttackTarget) // 공격을 진행
					.end()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::CheckFarSpawnPoistion) // 스폰 지점으로부터 너무 멀리 왔는지
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::MoveToTarget) // 타켓쪽으로 계속 움직여야 한다
					.end()
					.leaf<ActionNode>(m_actor->AI(), &ActorAI::ReturnRoutePosition) // 마지막 route 위치로 귀환
				.end()
			.end()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::NextRoute) // 다음 route 위치로 이동
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
