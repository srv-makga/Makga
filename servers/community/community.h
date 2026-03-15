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

	// ChatServer 연동 훅 — 파티/길드가 각자 오버라이드
	virtual void OnChatChannelCreate() {}
	virtual void OnChatChannelDestroy() {}

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
	UserUid GetLeaderUid() const;
	bool TransferLeader(UserUid new_leader_uid);

	bool IsMember(UserUid user_uid) const;
	bool IsFullMember() const;

	void ForEachMember(std::function<void(std::shared_ptr<CommunityMember>)> func) const;

	bool IsPublic() const;
	void SetPublic(bool is_public);

	bool Invite(UserUid user_uid);
	bool CancelInvite(UserUid user_uid);
	bool IsInvited(UserUid user_uid) const;
	bool AcceptInvite(UserUid invitee_uid, std::shared_ptr<CommunityMember> member);
	bool DeclineInvite(UserUid user_uid);

	Time GetCreatedTime() const;

protected:
	Id id_;
	Time created_time_ = 0;
	bool is_public_ = true;

	mutable makga::lib::SharedMutex members_mutex_;
	std::unordered_map<UserUid, std::shared_ptr<CommunityMember>> members_;
	UserUid leader_uid_ = 0;

	mutable makga::lib::SharedMutex invites_mutex_;
	std::unordered_set<UserUid> pending_invites_;
};