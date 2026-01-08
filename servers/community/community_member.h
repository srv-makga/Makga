#pragma once

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

protected:
	Id id_;
	UserUid user_uid_;
	std::weak_ptr<Community> community_;
};