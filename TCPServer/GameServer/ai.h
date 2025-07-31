#pragma once

#include "../Core/BrainTree.h"
#include "actor.h"

class AI : public core::ai::Node
{
public:
	AI(std::shared_ptr<Actor> _owner);
	virtual ~AI();

	void terminate(Status s) override;

	std::shared_ptr<Actor> Owner() const;
	Node::Ptr Root() const;

protected:
	std::shared_ptr<Actor> m_owner;
	Node::Ptr m_root;
};