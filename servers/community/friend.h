#pragma once

#include "community.h"
#include "friend_member.h"

// @brief 각 유저가 하나씩 소유하는 친구 목록 Community.
// leader_uid_ = 본인 UserUid, members_ = 친구 목록.
// 양방향 처리는 FriendManager가 담당.
class Friend : public Community
{
public:
	static constexpr std::size_t MaxFriends = 200;

	Friend();
	virtual ~Friend();

	bool Initialize(UserUid owner_uid);
	void Finalize();

public:
	void OnJoinedMember(std::shared_ptr<CommunityMember> member) override;
	void OnLeavdMember(std::shared_ptr<CommunityMember> member) override;
	void OnUpdate() override;

	std::size_t GetMaxMemberCount() const override;

public:
	// @brief 친구 신청 (= Invite 래퍼)
	bool RequestFriend(UserUid target_uid);

	// @brief 비대칭 차단 — 본인 목록에서만 state = Blocked
	bool BlockUser(UserUid user_uid);
	bool UnblockUser(UserUid user_uid);
	bool IsBlocked(UserUid user_uid) const;

	// @brief 친구에게 이동 요청 — CommunityServer → GameServer 텔레포트 패킷 전달
	// GameServer가 아이템/재화 소모 처리
	bool TravelToFriend(UserUid target_uid);
};
