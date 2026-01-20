#pragma once

#include <memory>

class Actor;
class Stat;

// 외부에 공개할 데이터
class ActorBoard
{
public:
	ActorBoard();
	virtual ~ActorBoard();

	// @brief 내 정보를 target_board 으로 복사
	void CopyTo(std::shared_ptr<ActorBoard> target_board);

public:
	int64_t hp_;
};