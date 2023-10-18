#pragma once

#include "BrainTree.h"
#include <functional>

class actor
{
public:
	bool has_target() { return true; }
	bool attack_target() { return true; }
	bool is_die_target() { return true; }
};

template<typename T = std::function<core::ai::Node::Status(void)>>
class action_node : public core::ai::Leaf
{
	T m_action;

public:
	action_node(T&& _action)
		: m_action(std::move(_action))
	{}
	~action_node()
	{}

	Status update() override
	{
		return m_action();
	}
};