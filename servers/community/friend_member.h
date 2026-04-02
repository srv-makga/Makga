#pragma once

#include "community_member.h"

// ─── FriendState ───────────────────────────────────────────────
enum class FriendState { Pending, Accepted, Blocked };

// ─── FriendMember ──────────────────────────────────────────────
class FriendMember : public CommunityMember
{
public:
	FriendMember();
	virtual ~FriendMember();

	FriendState GetState() const;
	void SetState(FriendState state);

	const std::string& GetMemo() const;
	bool SetMemo(const std::string& memo); // 50자 초과 시 false

	// @brief GameServer 위치 브로드캐스트 수신 시 갱신
	void UpdateLocation(uint32_t map_id, float x, float y, float z);
	uint32_t GetMapId() const;
	float GetX() const;
	float GetY() const;
	float GetZ() const;

private:
	static constexpr std::size_t MaxMemoLength = 50;

	FriendState state_ = FriendState::Pending;
	std::string memo_;

	uint32_t cached_map_id_ = 0;
	float cached_x_ = 0.f;
	float cached_y_ = 0.f;
	float cached_z_ = 0.f;
};
