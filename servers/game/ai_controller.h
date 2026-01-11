#pragma once

#include "actor.h"
#include "bt_node.h"

class AIController : public std::enable_shared_from_this<AIController>
{
public:
	using Node = BtNode<AIController>;

	AIController(std::shared_ptr<Actor> actor);
	virtual ~AIController();

	bool Initialize();
	void Finalize();

	void Update(float delta_time);
	void Terminate();

public:
	static std::shared_ptr<Node> CreateAggressive(std::shared_ptr<Actor> actor);
	static std::shared_ptr<Node> CreateNonAggressive(std::shared_ptr<Actor> actor);

protected:
	makga::lib::Node::Status IsDead() const;
	makga::lib::Node::Status TryResurrection();
	makga::lib::Node::Status HasTarget();
	makga::lib::Node::Status IsInsideAttackRange();
	makga::lib::Node::Status AttackTarget();
	makga::lib::Node::Status CheckFarSpawnPoistion();
	makga::lib::Node::Status MoveToTarget();
	makga::lib::Node::Status ReturnRoutePosition();
	makga::lib::Node::Status Idle();

protected:
	std::shared_ptr<Node> bt_;

	std::shared_ptr<Actor> actor_;
	makga::lib::Node::Ptr root_;
};