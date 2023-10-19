#include "pch.h"
#include "user.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::server::SendPid_##pid, &User::On##pid);

core::Dispatcher<User::Pid_t, User::Function_t> User::s_dispatcher;

bool User::InitDispatcher()
{
	REG_DISPATCHER(LoginSecurity);
	REG_DISPATCHER(CharacterCreate);
	REG_DISPATCHER(CharacterDelete);
	REG_DISPATCHER(CharacterSelect);
	REG_DISPATCHER(CharacterLogout);

	return true;
}

bool User::OnLoginSecurity(NetPacket* _packet)
{
	return false;
}

bool User::OnCharacterCreate(NetPacket* _packet)
{
	return false;
}

bool User::OnCharacterDelete(NetPacket* _packet)
{
	return false;
}

bool User::OnCharacterSelect(NetPacket* _packet)
{
	return false;
}

bool User::OnCharacterLogout(NetPacket* _packet)
{
	return false;
}