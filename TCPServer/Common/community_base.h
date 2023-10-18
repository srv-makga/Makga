#pragma once

#include "common_header.h"

class CommunityMemberBase;

// @brief 커뮤니티 컨텐츠의 기본 인터페이스
class CommunityBase
{
public:
	using MemberIdentity_t = uint64_t;
	using MemberContainer_t = std::unordered_map<uint64_t, CommunityMemberBase*>;

	CommunityBase() = default;
	virtual ~CommunityBase() = default;

	virtual void initialize() = 0;
	// init info
	// init member...

	virtual std::size_t MaxMemberCount() const = 0;
	virtual std::size_t CurMemberCount() const = 0;
	bool IsFullMember() const { MaxMemberCount() <= CurMemberCount(); }

protected:
	virtual void OnJoinMember(CommunityMemberBase* _member) = 0;
	virtual void OnLeaveMember(CommunityMemberBase* _member) = 0;
	virtual void OnChangeLeader(CommunityMemberBase* _member) = 0;
	virtual void OnDestroy(CommunityMemberBase* _member) = 0;

private:
	MemberContainer_t m_members;
};