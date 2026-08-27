#pragma once

#include "../common/define.h"
#include "../common/item_object.h"

import makga.lib.pattern.singleton;

// @brief 몬스터 사망 시 드랍 테이블을 조회해 아이템을 생성하고, 킬러 캐릭터의 인벤토리로 전달한다.
class DropSystem final : public makga::lib::Singleton<DropSystem>
{
public:
	DropSystem();

	// @brief monster_idx 드랍 테이블을 조회해 확률 판정 후 아이템을 생성,
	//        killer_id Actor에게 ReceiveLootMessage로 전달한다.
	void ProcessDrop(MonsterIdx monster_idx, ActorId killer_id);

private:
	bool RollDrop(int32_t drop_rate) const;

	mutable std::mt19937 rng_;
};
