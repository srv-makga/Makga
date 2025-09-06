#include "pch.h"
#include "actor.h"
#include "actor_ai.h"
#include "ai_aggressive.h"
#include "ai_boss.h"
#include "ai_non_aggressive.h"
#include "ai_summon.h"
#include "terrain.h"
#include "utility.h"

ActorAI::ActorAI(std::shared_ptr<Actor> _actor, fb::eAiType _type)
	: m_actor(_actor)
	, m_node(nullptr)
{
}

ActorAI::~ActorAI()
{
	m_actor = nullptr;
	m_node = nullptr;
}

void ActorAI::Initialize()
{
	m_node = nullptr;
}

void ActorAI::Finalize()
{
	m_node = nullptr;
}

bool ActorAI::IsValid() const
{
	if (nullptr == m_node)
	{
		return false;
	}

	// @todo actor의 상태 체크가 필요하다
	if (nullptr == m_actor)
	{
		return false;
	}

	return true;
}

void ActorAI::OnUpdate()
{
	if (nullptr != m_node)
	{
		m_node->tick();
	}
}

bool ActorAI::Create()
{
	switch (Type())
	{
	case eAiType_Aggressive:
		m_node = std::make_shared<AIAggressive>(m_actor);
		break;
	case eAiType_NonAggressive:
		m_node = std::make_shared<AINonAggressive>(m_actor);
		break;
	case eAiType_Boss:
		m_node = std::make_shared<AIBoss>(m_actor);
		break;
	case eAiType_Summon:
		m_node = std::make_shared<AISummon>(m_actor);
		break;
	default:
		return false;
	}

	return nullptr != m_node;
}

ActionNode::Status ActorAI::IsDead()
{
	if (false == m_actor->IsDead())
	{
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::TryResurrection()
{
	if (false == m_actor->IsDead())
	{
		return ActionNode::Status::Failure;
	}

	if (false == m_actor->CanResurrecton())
	{
		return ActionNode::Status::Failure;
	}

	if (Result_t::eResult_Success != m_actor->UpdatePosition(m_actor->SpawnPosition(), eActorMoveEffect_Teleport))
	{
		return ActionNode::Status::Failure;
	}

	if (Result_t::eResult_Success != m_actor->DoResurrecton())
	{
		return ActionNode::Status::Failure;
	}

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
	auto target = m_actor->Target();
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
	auto target = m_actor->Target();
	if (nullptr == target)
	{
		return ActionNode::Status::Failure;
	}

	Result_t result = m_actor->DoAttack(target, m_actor->SkillIndex());
	if (eResult_Success == result)
	{
	}
	else
	{
		DLOG_DEBUG << LOG_RESULT(result) << " " << m_actor->ActorUid() << " is attack " << target->ActorUid();
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::CheckFarSpawnPoistion()
{
	// 일정 거리이상 스폰 위치에서 벗어날 수 없다
	// @todo 몬스터별로 거리를 갖자
	if (m_actor->MaxAroundDistance() < UTIL.CalcDistance(m_actor->Position(), m_actor->SpawnPosition()))
	{
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::MoveToTarget()
{
	auto target = m_actor->Target();
	if (nullptr == target)
	{
		return ActionNode::Status::Failure;
	}

	Result_t result = m_actor->DoMove(target->Position());
	if (eResult_Success != result)
	{
		LOG_ERROR << LOG_ACTOR(m_actor) << LOG_RESULT(result);
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::ReturnRoutePosition()
{
	// @todo 무적 상태로 설정해줘야 한다
	m_actor->SetPosition(m_actor->SpawnPosition());

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::FindTarget()
{
	Terrain* terrain = m_actor->GetTerrain();
	if (nullptr == terrain)
	{
		return ActionNode::Status::Failure;
	}

	// 어그로 목록에서 타겟을 선정했으면 성공
	if (true == m_actor->SelectTarget())
	{
		return ActionNode::Status::Success;
	}

	int search_filter = 0;
	ActorList actor_list;
	if (false == terrain->AroundList(m_actor, search_filter, actor_list))
	{
		return ActionNode::Status::Failure;
	}

	// @todo actor_list 에서 가장 가까운 적 찾기
	Distance_t min_distance = 0.f;
	std::shared_ptr<Actor> target = nullptr;

	for (auto actor : actor_list)
	{
		Coord_t distance = UTIL.CalcDistance(m_actor->Position(), actor->Position());
		if (nullptr != actor || min_distance > distance)
		{
			min_distance = distance;
			target = actor;
		}
	}

	if (nullptr == target)
	{
		return ActionNode::Status::Failure;
	}

	m_actor->SetTarget(target);

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::SetTarget()
{
	if (false == m_actor->SelectTarget())
	{
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::Idle()
{
	if (false == m_actor->HasNextRoutePosition())
	{
		return ActionNode::Status::Failure;
	}

	m_actor->SetPosition(m_actor->NextRoutePosition());

	return ActionNode::Status::Success;
}

eAiType ActorAI::Type() const
{
	return m_actor->AIType();
}