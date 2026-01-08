#pragma once

class CommunityMember
{
public:
	using Id = uint64_t;

	CommunityMember();
	virtual ~CommunityMember();

	Id GetId() const { return id_; }
	void SetId(Id id) { id_ = id; }

protected:
	Id id_;
};
