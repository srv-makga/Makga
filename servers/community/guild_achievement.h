#pragma once

#include "guild_inventory.h"

// ─── AchievementType ───────────────────────────────────────────
enum class AchievementType
{
	Attendance, // 출석 (접속 인원 누적)
	MonsterKill, // 몬스터 처치
	DungeonClear, // 인스턴스 던전 클리어
};

struct AchievementReward
{
	TableIdx item_idx = 0; // 지급 아이템 (0 = 없음)
	StackCount count = 0;
	int64_t currency = 0; // 길드 인벤토리에 추가할 재화 (0 = 없음)
};

// @brief 1주일 단위로 초기화되는 길드 공동 업적
struct GuildAchievement
{
	uint32_t id = 0;
	AchievementType type;
	std::string name;
	int32_t goal = 0;
	int32_t progress = 0;
	bool completed = false;
	AchievementReward reward;

	bool IsComplete() const { return progress >= goal; }
	void Reset() { progress = 0; completed = false; }
};
