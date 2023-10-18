#pragma once

#include "user.h"
#include "../Common/manager_base.hpp"
#include "../Core/singleton.hpp"

class UserManager : public core::pattern::Singleton<UserManager>, ManagerBase<UserUid_t, User*>
{
public:
	UserManager() = default;
	virtual ~UserManager() = default;
};