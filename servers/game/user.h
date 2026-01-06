#pragma once

#include "user_dispatcher.h"

class User : public std::enable_shared_from_this<User>
{
public:
	User();
	virtual ~User();

	bool Initialize();
	void Finalize();

protected:
	UserUid user_uid_;
	UserDispatcher dispatcher_;
};