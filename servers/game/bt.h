#pragma once

#include "actor.h"

import makga.lib.ai;

class Bt : public makga::lib::Node
{
public:
	Bt(std::shared_ptr<Actor> actor);
	virtual ~Bt();

	bool Initialize();
	void Finalize();

	std::shared_ptr<Actor> GetActor() const;
	Node::Ptr GetRootNode() const;

protected:
	std::shared_ptr<Actor> actor_;
	Node::Ptr root_node_;
};