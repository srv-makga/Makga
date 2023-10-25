#pragma once

#include "../Core/BrainTree.h"
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