#pragma once

#include "bt_actor.h"

import makga.lib.ai;

template<typename T>
class BtNode : public makga::lib::Node
{
public:
	using Action = std::function<makga::lib::Node::Status(std::weak_ptr<T> actor)>;

	BtNode(std::weak_ptr<T> actor, Action&& action)
		: actor_(actor)
		, action_(std::move(action))
	{
	}

	virtual ~BtNode() = default;

public: // Node
	Status update() override
	{
		if (actor_.expired())
		{
			return Status::Invalid;
		}

		if (nullptr == action_)
		{
			return Status::Invalid;
		}

		return action_(actor_);
	}

protected:
	std::weak_ptr<T> actor_;
	Action action_;
};