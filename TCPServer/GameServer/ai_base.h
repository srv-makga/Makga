#pragma once

#include "../Core/BrainTree.h"
#include "actor.h"

// 각 행위 클래스들을 아래에 정리

class IsMoveAbleNode : public core::ai::Node
{
public:
	IsMoveAbleNode(Actor* _actor)
		: m_actor(_actor) {}
	~IsMoveAbleNode() = default;

	Status update() override
	{
		if (false == m_actor->IsMovable())
		{
			return Status::Failure;
		}

		return Status::Success;
	}

private:
	Actor* m_actor;
};
class HasTargetNode : public core::ai::Node
{
public:
	HasTargetNode(Actor* _actor)
		: m_actor(_actor) {}
	~HasTargetNode() = default;

	Status update() override
	{
		if (false == m_actor->HasTarget())
		{
			return Status::Failure;
		}

		return Status::Success;
	}

private:
	Actor* m_actor;
};
class MoveNextPos : public core::ai::Node
{
public:
	MoveNextPos(Actor* _actor)
		: m_actor(_actor) {}
	~MoveNextPos() = default;

	Status update() override
	{
		m_actor->CalculateNextPos();

		m_actor->Move(m_actor->NextPos());

		return Status::Success;
	}

private:
	Actor* m_actor;
};

class CanAttackNode : public core::ai::Node
{};
class AttackNode : public core::ai::Node
{};