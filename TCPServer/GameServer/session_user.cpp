#include "pch.h"
#include "session_user.h"
#include "user.h"
#include "pool.h"

//#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::server::SendPid_##pid, &SessionUser::On##pid);
#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::server::SendPid_##pid, [](SessionUser* _session, std::shared_ptr<Packet> packet) -> bool { return _session->On##pid(packet); });

core::Dispatcher<SessionUser::Pid_t, SessionUser::Function_t> SessionUser::s_dispatcher;

bool SessionUser::InitDispatcher()
{
	REG_DISPATCHER(LoginAuth);
	
	return true;
}

SessionUser::SessionUser()
	: IocpSession(Session::Type::IOCPClient, CONFIG.buffer_size_user)
	, m_user(nullptr)
{
}

SessionUser::~SessionUser()
{
}

bool SessionUser::Initialize()
{
	m_user = nullptr;

	return true;
}

void SessionUser::Finalize()
{
}

User* SessionUser::GetUser() const
{
	return m_user;
}

void SessionUser::SetUser(User* _user)
{
	if (m_user == _user)
	{
		return;
	}

	if (nullptr != m_user)
	{
		m_user->SetSession(nullptr);
	}

	m_user = _user;

	if (nullptr != _user)
	{
		_user->SetSession(this);
	}
}

ThreadId_t SessionUser::ThreadId() const
{
	return 0;
}

void SessionUser::OnConnected()
{
	LOG_INFO << "SessionId:" << GetSessionId() << ", UserSession Connected.";
}

void SessionUser::OnDisconnected()
{
	LOG_INFO << "SessionId:" << GetSessionId() << ", UserSession Disconnected.";
}

std::size_t SessionUser::OnRecv(char* _buffer, std::size_t _length)
{
	auto packet = POOL.packet.Pop();
	packet->SetBuffer(m_recv_buffer);

	ProcPacket(packet);

	return _length;
}

bool SessionUser::ProcPacket(std::shared_ptr<NetPacket> _packet)
{
	fb::server::SendPid pid = static_cast<fb::server::SendPid>(_packet->GetId());

	LOG_INFO << "ProcPacket. Pid:" << fb::server::EnumNameSendPid(pid);

	bool ret = s_dispatcher.Exec(pid, this, _packet);
	
	LOG_INFO << "ProcPacket. Pid:" << fb::server::EnumNameSendPid(pid) << ", ret:" << ret ? "true" : "false";

	return ret;
}

bool SessionUser::OnLoginAuth(std::shared_ptr<NetPacket> _packet)
{
	return false;
}