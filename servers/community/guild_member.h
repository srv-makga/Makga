#pragma once

#include "community_member.h"
#include "party.h"	// InstanceDungeonInfo 재사용

// ─── GuildGrade ────────────────────────────────────────────────
enum class GuildGrade { Master, SubMaster, Member };

// ─── GuildMember ───────────────────────────────────────────────
class GuildMember : public CommunityMember
{
public:
	GuildMember();
	virtual ~GuildMember();

	GuildGrade GetGrade() const;
	void       SetGrade(GuildGrade grade);
	bool       IsGuildMaster() const;

	Exp  GetContribution() const;
	void AddContribution(Exp amount);

	Level GetCachedLevel() const;
	void  SetCachedLevel(Level level);

	const std::string& GetPersonalMemo() const;
	bool SetPersonalMemo(const std::string& memo);   // 100자 초과 시 false

	// 등급별 권한
	bool CanEditNotice() const;
	bool CanKickMember() const;
	bool CanInvite() const;
	bool CanAccessStorage() const;

private:
	static constexpr std::size_t MaxMemoLength = 100;

	GuildGrade  grade_         = GuildGrade::Member;
	Exp         contribution_  = 0;
	Level       cached_level_  = 1;
	std::string personal_memo_;
};
