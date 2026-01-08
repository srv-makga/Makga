#pragma once

#include "community_member.h"

import makga.lib.lock;

class Community
{
public:
	using Id = uint64_t;

	Community() = default;
	virtual ~Community() = default;

public:
	Id GetId() const { return id_; }
	void SetId(Id id) { id_ = id; }

	bool AddMember(std::shared_ptr<CommunityMember> member);
	bool RemoveMember(CommunityMember::Id member_id);
	std::shared_ptr<CommunityMember> GetMember(CommunityMember::Id member_id);
	std::size_t GetMemberCount() const;

protected:
	Id id_;

	mutable makga::lib::SharedMutex members_mutex_;
	std::unordered_map<CommunityMember::Id, std::shared_ptr<CommunityMember>> members_;
};