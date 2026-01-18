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

protected: // 행동 트리 노드들
	// @brief 액터가 죽었는지 판단
	makga::lib::Node::Status IsDead();
	// @brief 부활 시도
	makga::lib::Node::Status TryResurrection();
	// @brief 마을 귀환
	makga::lib::Node::Status ReturnCity();
	// @brief 공격 범위 내인지 판단
	makga::lib::Node::Status IsInsideAttackRange();
	// @brief 타겟 공격
	makga::lib::Node::Status AttackTarget();
	// @brief 도망가기
	makga::lib::Node::Status RunAway();
	// @brief 스폰 위치에서 멀리 떨어졌는지 판단
	makga::lib::Node::Status IsInFromSpawnPoint();
	// @brief 타겟 위치로 이동
	makga::lib::Node::Status MoveToTarget();
	// @brief 스폰 위치로 복귀
	makga::lib::Node::Status ReturnSpawnPosition();
	// @brief 순회 위치로 복귀
	makga::lib::Node::Status ReturnRoutePosition();
	// @brief 이동하려는 경로 순회
	makga::lib::Node::Status Patrol();
	// @brief 유휴 상태
	makga::lib::Node::Status Idle();
	// @brief 제자리 서있기
	makga::lib::Node::Status Stand();
	// @brief 타겟이 있는지 판단
	makga::lib::Node::Status HasTarget();
	// @brief 타겟 찾기
	makga::lib::Node::Status FindTarget();
	// @brief 타겟 설정
	makga::lib::Node::Status SetTarget();

protected:
	static std::shared_ptr<makga::lib::Node> CreateAggressive(AIController* ptr);
	static std::shared_ptr<makga::lib::Node> CreateNonAggressive(AIController* ptr);
	static std::shared_ptr<makga::lib::Node> CreateCoward(AIController* ptr);
	static std::shared_ptr<makga::lib::Node> CreateFieldBoss(AIController* ptr);
	static std::shared_ptr<makga::lib::Node> CreateInstanceBoss(AIController* ptr);
	static std::shared_ptr<makga::lib::Node> CreateCharacter(AIController* ptr);

protected:
	std::shared_ptr<Actor> actor_;
	makga::lib::Node::Ptr root_;
};