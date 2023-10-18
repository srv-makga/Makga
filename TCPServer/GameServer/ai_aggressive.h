#pragma once

#include "base_ai.h";
class ai_aggressive : public core::ai::Node
{
	actor* m_actor;
	Node::Ptr m_root = nullptr;

public:
	ai_aggressive(actor* _actor)
		: m_actor(_actor)
		, m_root(nullptr) {}
	~ai_aggressive() {}

	void Initialize() override
	{
		m_root = core::ai::Builder()
			.decorator<core::ai::Repeater>()
				.composite<core::ai::Sequence>()
					.leaf<action_node<>>([this]()->core::ai::Node::Status { return m_actor->has_target() ? core::ai::Node::Status::Success : core::ai::Node::Status::Failure; })
					.leaf<action_node<>>([this]()->core::ai::Node::Status { return m_actor->attack_target() ? core::ai::Node::Status::Success : core::ai::Node::Status::Failure; })
					.leaf<action_node<>>([this]()->core::ai::Node::Status { return m_actor->is_die_target() ? core::ai::Node::Status::Success : core::ai::Node::Status::Failure; })
				.end()
			.end()
			.build();
	}

	void terminate(Status s) override
	{
		switch (s)
		{

		}
	}

	Status update() override
	{
		m_root->tick();
	}

	// 아래에 필요한 함수 구현 후 Initialize()에서 활용

	const core::ai::Node::Ptr root() { return m_root; }
};