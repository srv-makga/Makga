#pragma once

#include "community_member.h"

class Community
{
public:
	using Id = uint64_t;

	Community();
	virtual ~Community();

public:
	virtual void OnJoinedMember(std::shared_ptr<CommunityMember> member) = 0;
	virtual void OnLeavdMember(std::shared_ptr<CommunityMember> member) = 0;
	virtual void OnUpdate() = 0;

	virtual std::size_t GetMaxMemberCount() const = 0;

public:
	bool Initialize();
	void Finalize();

	Id GetId() const;
	void SetId(Id id);

	bool AddMember(std::shared_ptr<CommunityMember> member);
	bool RemoveMember(UserUid user_uid);
	std::shared_ptr<CommunityMember> GetMember(UserUid user_uid);
	std::size_t GetMemberCount() const;

	void SetLeader(UserUid user_uid);

	bool IsMember(UserUid user_uid) const;
	bool IsFullMember() const;

protected:
	Id id_;

	mutable makga::lib::SharedMutex members_mutex_;
	std::unordered_map<UserUid, std::shared_ptr<CommunityMember>> members_;
	UserUid leader_uid_;
};