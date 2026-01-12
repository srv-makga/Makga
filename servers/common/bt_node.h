#pragma once

import makga.lib.ai;

template<typename T>
class BtNode : public makga::lib::Node
{
public:
	using Action = std::function<makga::lib::Node::Status(T*)>;

	BtNode(T* actor, Action action)
		: actor_(actor)
		, action_(action)
	{
	}

	virtual ~BtNode() = default;

public: // Node
	Status update() override
	{
		if (nullptr == actor_)
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
	T* actor_;
	Action action_;
};