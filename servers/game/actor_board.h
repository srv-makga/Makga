#pragma once

#include <memory>

import makga.math.vector3;

class Actor;
class Stat;

// 외부에 공개할 데이터
class ActorBoard
{
	friend class Actor;

public:
	ActorBoard();
	virtual ~ActorBoard();

	// @brief 내 정보를 target_board 으로 복사
	void CopyTo(ActorBoard* target_board);

public:
	Hp cur_hp_;
	Hp max_hp_;
	
	Mp cur_mp_;
	Mp max_mp_;

	std::unique_ptr<Stat> stat_;

	makga::math::Vector3 position_;
};