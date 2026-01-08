#pragma once

#include "community.h"

class Party : public Community
{
public:
	Party();
	virtual ~Party();

	bool Initialize();
	void Finalize();

public:
	void OnJoinedMember(std::shared_ptr<CommunityMember> member) override;
	void OnLeavdMember(std::shared_ptr<CommunityMember> member) override;
	void OnUpdate() override;

	std::size_t GetMaxMemberCount() const override;
};