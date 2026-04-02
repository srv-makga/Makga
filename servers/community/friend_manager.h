#pragma once

#include "friend.h"
#include "friend_member.h"

// @brief 양방향 친구 관계의 원자적 처리 담당.
// 두 UserUid 를 항상 작은 값 순으로 정렬하여 락 획득 — 데드락 방지.
class FriendManager
{
public:
	// @brief A가 B에게 친구 신청. B의 Friend에 Invite(A) 추가.
	bool RequestFriend(Friend& from_list, Friend& to_list, UserUid from, UserUid to);

	// @brief B가 수락. A.Friend AddMember(B) + B.Friend AddMember(A) 동시 처리.
	bool AcceptFriend(Friend& acceptor_list, Friend& requester_list,
		UserUid acceptor, UserUid requester,
		std::shared_ptr<FriendMember> acceptor_as_member,
		std::shared_ptr<FriendMember> requester_as_member);

	// @brief B가 거절. B의 pending_invites_ 에서 A 제거.
	bool DeclineFriend(Friend& decliner_list, UserUid requester);

	// @brief 친구 삭제. A.Friend RemoveMember(B) + B.Friend RemoveMember(A) 동시 처리.
	bool RemoveFriend(Friend& a_list, Friend& b_list, UserUid a, UserUid b);

	// @brief A가 B를 차단. A.Friend 에서만 state = Blocked (비대칭).
	bool BlockFriend(Friend& blocker_list, UserUid target);

	// @brief 친구에게 이동 요청. GameServer 텔레포트 요청 패킷 전달.
	bool TravelToFriend(Friend& self_list, UserUid self_uid, UserUid target_uid);

private:
	// @brief 두 Friend 객체를 leader_uid 작은 순서로 잠금.
	// 반환값: {first, second} — 호출자가 같은 순서로 잠금 해제.
	static std::pair<Friend*, Friend*> lock_order(Friend& a, Friend& b);
};
