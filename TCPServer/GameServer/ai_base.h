#pragma once

#include "../Core/BrainTree.h"
#include "ai_aggressive.h"
#include "ai_non_aggressive.h"
#include "ai_boss.h"

class Actor;

// @brief ai 인터페이스 클래스

#include <functional>

template<typename T = std::function<core::ai::Node::Status(void)>>
class ActionNode : public core::ai::Leaf
{
public:
	ActionNode(T&& _action)
		: m_action(std::move(_action))
	{}
	~ActionNode()
	{}

	Status update() override
	{
		return m_action();
	}

private:
	T m_action;
};