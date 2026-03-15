#pragma once
class Community;

class CommunityMember
{
public:
	using Id = uint64_t;

	CommunityMember() = default;
	virtual ~CommunityMember() = default;

	Id GetId() const;
	void SetId(Id id);

	UserUid GetUserUid() const;
	void SetUserUid(UserUid user_uid);

	std::shared_ptr<Community> GetCommunity() const;
	void SetCommunity(std::weak_ptr<Community> community);

	Time GetJoinTime() const;
	void SetJoinTime(Time t);

	Time GetLastLoginTime() const;
	void UpdateLastLoginTime();

	bool IsOnline() const;
	void SetOnline(bool online);

protected:
	Id id_ = 0;
	UserUid user_uid_ = 0;
	std::weak_ptr<Community> community_;

	Time join_time_ = 0;
	Time last_login_time_ = 0;
	bool is_online_ = false;
};