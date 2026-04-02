#pragma once

#include "community_member.h"
#include "party.h"

class PartyMember : public CommunityMember
{
public:
	PartyMember();
	virtual ~PartyMember();

	makga::PartyRole GetRole() const;
	void SetRole(makga::PartyRole role);
	bool IsLeader() const;

	bool IsReady() const;
	void SetReady(bool ready);

	float GetHpRatio() const;
	void SetHpRatio(float ratio);

private:
	makga::PartyRole role_ = makga::PartyRole_Member;
	bool ready_ = false;
	float hp_ratio_ = 1.0f;
};
