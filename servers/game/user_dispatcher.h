#pragma once

#include "dispatcher.h"
#include "user.h"

class UserDispatcher : public DispatcherTemplate<User>
{
public:
	UserDispatcher(std::shared_ptr<User> owner);
	virtual ~UserDispatcher() = default;

	bool Execute(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet) override;
};