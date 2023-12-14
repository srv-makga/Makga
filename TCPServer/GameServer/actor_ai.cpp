#include "pch.h"
#include "actor_ai.h"
#include "ai_aggressive.h"
#include "ai_non_aggressive.h"
#include "ai_boss.h"
#include "actor.h"
#include "actor_manager.h"
#include "data_manager.h"
#include "utility.h"

ActorAI::ActorAI(Actor* _actor, fb::eAiType _type)
	: m_actor(_actor)
	, m_node(nullptr)
{
}

ActorAI::~ActorAI()
{
	delete m_node;
}

void ActorAI::Initialize()
{
}

void ActorAI::OnUpdate()
{
	if (m_node)
	{
		m_node->tick();
	}
}

bool ActorAI::Create()
{
	switch (Type())
	{
	case eAiType_Aggressive:
		m_node = new AIAggressive(m_actor);
		break;
	case eAiType_NonAggressive:
		m_node = new AINonAggressive(m_actor);
		break;
	case eAiType_Boss:
		m_node = new AIBoss(m_actor);
		break;
	default:
		return false;
	}

	return true;
}

ActionNode::Status ActorAI::CheckResurrection()
{
	if (false == m_actor->IsDead())
	{
		return ActionNode::Status::Failure;
	}

	if (false == m_actor->CanResurrecton())
	{
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::Resurrection()
{
	m_actor->SetDefaultPosition();
	m_actor->Resurrecton();
	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::HasTarget()
{
	if (false == m_actor->HasTarget())
	{
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::IsInsideAttackRange()
{
	Actor* target = m_actor->Target();
	if (nullptr == target)
	{
		return ActionNode::Status::Failure;
	}

	Coord_t distance = UTIL.CalcDistance(m_actor->Position(), target->Position());
	if (m_actor->AttackRange() < distance)
	{
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::AttackTarget()
{
	Actor* target = m_actor->Target();
	if (nullptr == target)
	{
		return ActionNode::Status::Failure;
	}

	// 내가 공격할 수 없는 상태
	if (false == m_actor->IsAttackable())
	{
		return ActionNode::Status::Failure;
	}

	// 상대방이 공격 당할 수 없는 상태
	if (false == target->IsAttacked())
	{
		return ActionNode::Status::Failure;
	}

	Result_t result = target->Attacked(m_actor);
	if (eResult_Success != result)
	{
		DLOG_DEBUG << LOG_RESULT(result) << " " << m_actor->Uid() << " is attack " << target->Uid();
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::CheckFarSpawnPoistion()
{
	// 일정 거리이상 스폰 위치에서 벗어날 수 없다
	if (SYSTEM.actor.max_around_distance < UTIL.CalcDistance(m_actor->Position(), m_actor->LastRoutePosition()))
	{
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::MoveToTarget()
{
	Actor* target = m_actor->Target();
	if (nullptr == target)
	{
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status();
}

ActionNode::Status ActorAI::ReturnRoutePosition()
{
	m_actor->SetPosition(m_actor->LastRoutePosition());

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::NextRoute()
{
	if (false == m_actor->HasNextRoutePosition())
	{
		return ActionNode::Status::Failure;
	}

	m_actor->SetPosition(m_actor->NextRoutePosition());

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::CheckSearchProcess()
{
	if (0 == m_actor->SpawnAggro())
	{
		return ActionNode::Status::Failure;
	}

	if (eAiType_NonAggressive == m_actor->AIType())
	{
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::RunSearchProcess()
{
	if (0 < m_actor->SpawnAggro())
	{
		// 주변에 있는가 & 공격 가능한 상태인가
		// 어그로 목록에 등록
		// 아니라면 주변 탐색

		do
		{
			Actor* target = ACTOR_MANAGER.Find(m_actor->SpawnAggro()->ActorId());
			if (nullptr == target)
			{

			}

			m_actor->AddAggroList(m_actor->SpawnAggro());
		}
	}
	else
	{
		Actor* target = m_actor->FindEnermy();
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::CheckGuardProcess()
{
	return ActionNode::Status();
}

ActionNode::Status ActorAI::RunGuardProcess()
{
	return ActionNode::Status();
}

ActionNode::Status ActorAI::CheckFollowProcess()
{
	return ActionNode::Status();
}

ActionNode::Status ActorAI::RunFollowProcess()
{
	return ActionNode::Status();
}

ActionNode::Status ActorAI::CheckMoveProcess()
{
	return ActionNode::Status();
}

ActionNode::Status ActorAI::RunMoveProcess()
{
	return ActionNode::Status();
}

eAiType ActorAI::Type() const
{
	return m_actor->AIType();
}