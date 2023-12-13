#include "pch.h"
#include "actor_ai.h"
#include "ai_aggressive.h"
#include "ai_non_aggressive.h"
#include "ai_boss.h"
#include "actor.h"
#include "actor_manager.h"
#include "../Core/math.h"

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

	Coord_t distance_square = MATH.DistanceSquare(m_actor->Position().x,
		m_actor->Position().y,
		m_actor->Position().z,
		target->Position().x,
		target->Position().y,
		target->Position().z
	);

	if (m_actor->AttackRange() < distance_square)
	{
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::NextRoute()
{
	return ActionNode::Status();
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