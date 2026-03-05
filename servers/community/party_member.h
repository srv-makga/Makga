#pragma once

#include "community_member.h"
#include "party.h"

class PartyMember : public CommunityMember
{
public:
	PartyMember();
	virtual ~PartyMember();

	PartyRole GetRole() const;
	void      SetRole(PartyRole role);
	bool      IsLeader() const;

	bool IsReady() const;
	void SetReady(bool ready);

	float GetHpRatio() const;
	void  SetHpRatio(float ratio);

private:
	PartyRole role_     = PartyRole::Member;
	bool      ready_    = false;
	float     hp_ratio_ = 1.0f;
};