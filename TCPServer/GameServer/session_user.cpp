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

SessionUser::SessionUser(std::size_t _buffer_size)
	: IocpSession(core::ServiceType::IOCP_CLIENT, _buffer_size)
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

void SessionUser::ProcConnect()
{
	LOG_INFO << "SessionId:" << GetSessionId() << ", UserSession Connected.";
}

void SessionUser::ProcDisconnect()
{
	LOG_INFO << "SessionId:" << GetSessionId() << ", UserSession Disconnected.";

	// @todo 세션을 종료했을 떄 IOCP로 close 이벤트가 통지되고
	// 소켓이 종료된 채로 여기에 들어오는 건지, 이벤트만 발생하고 여기서 종료 처리 해야되는지 확인 필요
}

std::size_t SessionUser::ProcRecv(char* _data, std::size_t _recv_size)
{
	if (nullptr == _data)
	{
		return 0;
	}

	std::shared_ptr<Packet> packet = Packet::Pop();
	packet->SetData(reinterpret_cast<char*>(_data), _recv_size);

	ProcPacket(packet);
	// @todo 패킷 반납처리 (패킷 삭제처리자에 의해 반납되어야함)
	
	return _recv_size; // 패킷 처리에 성공하면 수신된 크기를 반환한다
}

void SessionUser::ProcSend(std::size_t _sent_size)
{
}

void SessionUser::ProcPacket(std::shared_ptr<NetPacket> _packet)
{
	if (true == _packet->IsEncrypt())
	{
		// @todo ENCRYPTER->Decrypt(_packet->Data(), _packet->DataSize(), _packet->GetEncryptKey());
	}

	fb::server::SendPid pid = static_cast<fb::server::SendPid>(_packet->GetId());

	LOG_INFO << "ProcPacket. Pid:" << fb::server::EnumNameSendPid(pid);

	bool ret = s_dispatcher.Exec(pid, this, _packet);
	
	LOG_INFO << "ProcPacket. Pid:" << fb::server::EnumNameSendPid(pid) << ", ret:" << ret ? "true" : "false";
}

bool SessionUser::OnLoginAuth(std::shared_ptr<NetPacket> _packet)
{
	return false;
}