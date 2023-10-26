#include "pch.h"
#include "ai_aggressive.h"
#include "actor.h"

AIAggressive::AIAggressive(Actor* _actor)
	: m_actor(_actor)
	, m_root(nullptr)
{
}

void AIAggressive::initialize()
{
	m_root = core::ai::Builder()
		.decorator<core::ai::Repeater>()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode<>>([this]()->core::ai::Node::Status { return m_actor->HasTarget() ? Status::Success : Status::Failure; })
				.leaf<ActionNode<>>([this]()->core::ai::Node::Status { return m_actor->Target() ? Status::Success : Status::Failure; })
				.leaf<ActionNode<>>([this]()->core::ai::Node::Status { return m_actor->Target()->IsDie() ? Status::Success : Status::Failure; })
			.end()
		.end()
		.build();
}

void AIAggressive::terminate(Status s)
{
	switch (s)
	{

	}
}

AIAggressive::Status AIAggressive::update()
{
	m_root->tick();

	return Status::Success;
}
