#pragma once

#include "bt_actor.h"

import makga.lib.ai;

class Bt : public makga::lib::Node
{
public:
	Bt(std::shared_ptr<BtActor> actor);
	virtual ~Bt();

	bool Initialize();
	void Finalize();

	std::shared_ptr<BtActor> GetActor() const;
	Node::Ptr GetRootNode() const;

protected:
	std::shared_ptr<BtActor> actor_;
	Node::Ptr root_node_;
};