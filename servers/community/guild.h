#pragma once

#include "community.h"
#include "guild_member.h"
#include "guild_inventory.h"
#include "guild_achievement.h"
#include "guild_shop.h"
#include "guild_craft.h"
#include "party.h"	// InstanceDungeonInfo 재사용

// ─── GuildRelationType ─────────────────────────────────────────
enum class GuildRelationType { Neutral, Allied, Rival };

using GuildId = Community::Id;

// ─── Guild ─────────────────────────────────────────────────────
class Guild : public Community
{
public:
	static constexpr std::size_t MaxMembers        = 100;
	static constexpr std::size_t MaxAchievements   = 20;

	Guild();
	virtual ~Guild();

	bool Initialize();
	void Finalize();

public:
	void OnJoinedMember(std::shared_ptr<CommunityMember> member) override;
	void OnLeavdMember(std::shared_ptr<CommunityMember> member) override;
	void OnUpdate() override;
	void OnChatChannelCreate() override;
	void OnChatChannelDestroy() override;
	virtual void OnAchievementComplete(const GuildAchievement& ach) {}

	std::size_t GetMaxMemberCount() const override;

public:
	// ─── 기본 정보 ───
	const std::string& GetName() const;
	bool SetName(UserUid requester, const std::string& name);   // Master 전용

	const std::string& GetMark() const;
	bool SetMark(UserUid requester, const std::string& mark);   // Master 전용

	const std::string& GetNotice() const;
	bool SetNotice(UserUid requester, const std::string& notice);   // Master/SubMaster

	Level GetLevel() const;
	void  AddExp(Exp amount);

	// ─── 등급 관리 ───
	bool SetMemberGrade(UserUid requester, UserUid target, GuildGrade grade);

	// ─── 서브시스템 접근 ───
	GuildInventory* GetInventory();
	GuildShop*      GetShop();
	GuildCraft*     GetCraft();

	// ─── 인스턴스 던전 ───
	bool EnterGuildDungeon(TableIdx dungeon_idx, uint64_t instance_id);
	bool LeaveGuildDungeon();
	bool IsInGuildDungeon() const;
	const InstanceDungeonInfo& GetGuildDungeonInfo() const;

	// ─── 타 길드 관계 ───
	bool              SetRelation(UserUid requester, GuildId target_guild, GuildRelationType type);
	GuildRelationType GetRelation(GuildId target_guild) const;

	// ─── 업적 ───
	void UpdateAchievementProgress(AchievementType type, int32_t delta);
	// @brief 주 단위 초기화
	void ResetWeeklyAchievements();
	void LoadAchievements(std::vector<GuildAchievement> achievements);

private:
	GuildMember* get_guild_member(UserUid user_uid);

	std::string  name_;
	std::string  mark_;
	std::string  notice_;
	Level        level_ = 1;
	Exp          exp_   = 0;

	std::unique_ptr<GuildInventory>        inventory_;
	std::unique_ptr<GuildShop>             shop_;
	std::unique_ptr<GuildCraft>            craft_;
	std::vector<GuildAchievement>          achievements_;
	InstanceDungeonInfo                    dungeon_info_;

	mutable makga::lib::SharedMutex                        relations_mutex_;
	std::unordered_map<GuildId, GuildRelationType> relations_;
};
