#pragma once

#include "bt_node.h"

class Actor;
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
	static std::shared_ptr<makga::lib::Node> CreateAggressive(std::weak_ptr<AIController> ptr);
	static std::shared_ptr<makga::lib::Node> CreateNonAggressive(std::weak_ptr<AIController> ptr);

protected: // 행동 트리 노드들
	// @brief 액터가 죽었는지 판단
	makga::lib::Node::Status IsDead() const;
	// @brief 부활 시도
	makga::lib::Node::Status TryResurrection();
	// @brief 공격 범위 내인지 판단
	makga::lib::Node::Status IsInsideAttackRange();
	// @brief 타겟 공격
	makga::lib::Node::Status AttackTarget();
	// @brief 스폰위치가 멀리 떨어져있는지 판단
	makga::lib::Node::Status IsFarFromSpawnPoint();
	// @brief 타겟 위치로 이동
	makga::lib::Node::Status MoveToTarget();
	// @brief 스폰위치로 복귀
	makga::lib::Node::Status ReturnRoutePosition();
	// @brief 유휴 상태
	makga::lib::Node::Status Idle();
	// @brief 타겟이 있는지 판단
	makga::lib::Node::Status HasTarget();
	// @brief 타겟 찾기
	makga::lib::Node::Status FindTarget();
	// @brief 타겟 설정
	makga::lib::Node::Status SetTarget();

protected:
	std::shared_ptr<Actor> actor_;
	makga::lib::Node::Ptr root_;
};