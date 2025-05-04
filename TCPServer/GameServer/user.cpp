#include "pch.h"
#include "user.h"
#include "session_user.h"
#include "server_game.h"
#include "pool.h"
#include "inventory_user.h"
#include "item_object_base.h"

const Messenger::MessageType& User::GetHandler(CommandType _type, PacketId_t _pid) const
{
	// using MessageType = std::function<bool(Packet*)>; => Messenger
	// using Function_t = std::function<bool(User*, std::shared_ptr<Packet>)>; => User
	// inline static std::map<CommandType, core::Dispatcher<Pid_t, Function_t>> s_dispatcher;
	const auto iter = s_dispatcher.find(_type);
	if (s_dispatcher.end() == iter)
	{
		return nullptr;
	}

	Pid_t pid = static_cast<Pid_t>(_pid);

	const auto func = iter->second.Find(pid);
	if (nullptr == func)
	{
		return nullptr;
	}

	func(this, )

	return func;
}

User::User()
{
}

User::~User()
{
}

void User::Initialize()
{
	m_account.Clear();
	m_auth_key.Clear();
	m_session = nullptr;
	m_max_inventory = 0;
	m_user_uid = 0;
}

void User::Finalize()
{
}

void User::Move(const Vector_t& _vec, Speed_t _speed, int _effect, int _animation)
{
}

const String8& User::Account() const
{
	return m_account;
}

SessionUser* User::Session() const
{
	return m_session;
}

void User::SetSession(SessionUser* _session)
{
	m_session = _session;
}

bool User::ProcPacket(std::shared_ptr<Packet> _packet)
{
	fb::server::SendPid pid = static_cast<fb::server::SendPid>(_packet->GetId());



	LOG_INFO << "ProcPacket Start. Pid:" << LOG_USER_UID(UserUid()) << fb::server::EnumNameSendPid(pid);

	bool ret = s_dispatcher.Exec(pid, this, _packet);

	LOG_INFO << "ProcPacket End. Pid:" << LOG_USER_UID(UserUid()) << fb::server::EnumNameSendPid(pid) << " result:" << ret ? "true" : "false";

	return ret;
}

bool User::Send(fb::server::RecvPid _pid, fbb& _fbb)
{
	if (nullptr == m_session)
	{
		LOG_ERROR << "m_session is nullptr" << LOG_USER(this);
		return false;
	}

	std::shared_ptr<core::network::NetBuffer> buffer = POOL.buffer.Pop();
	buffer->Write((char*)&_pid, sizeof(_pid));
	buffer->Write((char*)_fbb.GetBufferPointer(), _fbb.GetSize());

	m_session->Send(buffer);

	return true;
}

Character* User::ActiveCharacter() const
{
	return m_character;
}

void User::SetActiveCharacter(Character* _character)
{
	m_character = _character;
}