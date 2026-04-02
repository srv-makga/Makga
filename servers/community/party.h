#pragma once

#include "community.h"
#include "party_member.h"

struct JoinCondition
{
	Level min_level = 0; // 0 = 제한 없음
	Level max_level = 0; // 0 = 제한 없음

	bool Check(Level user_level) const
	{
		if (0 < min_level && user_level < min_level)
		{
			return false;
		}
		if (0 < max_level && user_level > max_level)
		{
			return false;
		}
		return true;
	}
};

struct PartyGoal
{
	makga::PartyGoalType type= makga::PartyGoalType_Field;
	uint32_t map_id = 0;
	std::string description;
};

struct InstanceDungeonInfo
{
	uint64_t instance_id = 0;
	TableIdx dungeon_idx = 0;
	Time enter_time = 0;
};

// ─── Party ─────────────────────────────────────────────────────
class Party : public Community
{
public:
	static constexpr std::size_t MaxMembers = 5;

	Party();
	virtual ~Party();

	bool Initialize();
	void Finalize();

public:
	void OnJoinedMember(std::shared_ptr<CommunityMember> member) override;
	void OnLeavdMember(std::shared_ptr<CommunityMember> member) override;
	void OnUpdate() override;
	void OnChatChannelCreate() override;
	void OnChatChannelDestroy() override;

	std::size_t GetMaxMemberCount() const override;

public:
	// 상태
	makga::PartyState GetState() const;
	void SetState(makga::PartyState state);

	makga::ExpShareType GetExpShareType() const;
	void SetExpShareType(makga::ExpShareType type);

	makga::LootType GetLootType() const;
	void SetLootType(makga::LootType type);

	// 목표 맵
	const PartyGoal& GetGoal() const;
	void SetGoal(const PartyGoal& goal);

	// 가입 조건
	const JoinCondition& GetJoinCondition() const;
	void SetJoinCondition(const JoinCondition& cond);
	bool CanJoin(Level user_level) const;

	// 채팅 채널 (ChatServer 생성 후 세팅)
	uint64_t GetChatChannelId() const;
	void SetChatChannel(uint64_t channel_id, const std::string& name);
	const std::string& GetChatChannelName() const;

	// 소환 (파티장 전용) — CommunityServer → GameServer 소환 요청 위임
	bool SummonMembers(UserUid leader_uid);

	// 인스턴스 던전
	bool EnterDungeon(TableIdx dungeon_idx, uint64_t instance_id);
	bool LeaveDungeon();
	bool IsInDungeon() const;
	const InstanceDungeonInfo& GetDungeonInfo() const;

	// 파티 홍보 — CommunityServer → ChatServer 메시지 전달 위임
	void BroadcastRecruitment(const std::string& channel);

private:
	makga::PartyState state_ = makga::PartyState_Idle;
	makga::ExpShareType exp_share_ = makga::ExpShareType_Equal;
	makga::LootType loot_type_ = makga::LootType_FreeForAll;
	PartyGoal goal_;
	JoinCondition join_condition_;
	InstanceDungeonInfo dungeon_info_;

	uint64_t chat_channel_id_ = 0;
	std::string chat_channel_name_;
};
