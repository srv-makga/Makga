#pragma once

#include "../Core/BrainTree.h"
#include "actor.h"

template <typename... Args>
class ActionNode : public core::ai::Node
{
	using T = Status(Actor::*);
public:
	ActionNode(Actor* _actor, T _action, Args... _args)
		: m_actor(_actor)
		, m_action(_action)
		, m_args(std::forward(_args))
	{}

	Status update() override
	{
		return m_actor->action(m_args);
	}

private:
	Actor* m_actor;
	T m_action;
};

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