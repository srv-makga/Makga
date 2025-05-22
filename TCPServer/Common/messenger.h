#pragma once

#include "common_header.h"
#include "../core/dispatcher.h"

// command를 보내는 타입
enum class CommandType
{
	World,
	Community,
	Game,
	User,
};

// @brief Packet Handler가 필요한 클래스의 인터페이스
template<typename Identifier, typename Message>
class Messenger
{
public:
	using Identifier_t = Identifier;
	using Message_t = Message; // std::function<bool(std::shared_ptr<Messenger>, std::shared_ptr<Packet>)>;
	using Dispatcher_t = core::Dispatcher<Identifier_t, Message_t>;

public:
	Messenger(std::map<CommandType, Dispatcher_t>& _dispatcher)
		: m_dispatcher(_dispatcher)
	{
	}
	virtual ~Messenger() = default;

	bool ProcPacket(CommandType _type, std::shared_ptr<Messenger<Identifier, Message>> _owner, Identifier_t _pid, std::shared_ptr<Packet> _packet)
	{
		auto iter = m_dispatcher.find(_type);
		if (m_dispatcher.end() == iter)
		{
			return false;
		}
		
		return iter->second.Exec(_pid, _owner, _packet);
	}

	virtual ThreadId_t GetThreadId() const { return 0; }

protected:
	std::map<CommandType, Dispatcher_t>& m_dispatcher;
};