#pragma once

#include "../common/dispatcher.h"

class User;
class UserDispatcher : public DispatcherTemplate<User>
{
public:
	UserDispatcher(std::shared_ptr<User> owner);
	virtual ~UserDispatcher() = default;

	bool Execute(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet) override;

private:
	// ── 로그인 시퀀스 ────────────────────────────────────────────────────────
	bool Login(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
	bool SelectCharacter(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

	// ── 인게임 ───────────────────────────────────────────────────────────────
	bool Move(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
};
