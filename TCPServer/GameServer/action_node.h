#pragma once

#include "../Core/BrainTree.h"
#include <functional>
#include <memory>

class ActorAI;

// @breif AI 라이브러리 사용을 위한 상속 클래스
class ActionNode : public core::ai::Node, public std::enable_shared_from_this<ActionNode>
{
	using ActionType = std::function<core::ai::Node::Status(ActorAI*)>;

public:
	ActionNode(std::shared_ptr<ActorAI> _ai, ActionType _action);
	virtual ~ActionNode() = default;

	core::ai::Node::Status update() override;

private:
	std::shared_ptr<ActorAI> m_ai;
	ActionType m_action;
};