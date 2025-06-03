#pragma once

#include "game_header.h"
#include "app_config.h"
#include "pool.h"

class AppSession : public Session_t
{
public:
	AppSession(core::ServiceType _type, std::size_t _buffer_size, eServerType _session_type)
		: Session_t(_type, _buffer_size)
		, m_session_type(_session_type)
	{
	}

	virtual ~AppSession() = default;

	void Send(PacketId_t _pid, FB_BUILDER& _fbb)
	{
		if (0 == _fbb.GetSize())
		{
			return;
		}

		std::shared_ptr<core::network::NetBuffer> send_data = POOL.buffer.Pop();

		send_data->Write((char*)&_pid, sizeof(_pid));
		send_data->Write(reinterpret_cast<char*>(_fbb.GetBufferPointer()), _fbb.GetSize());
		Session_t::Send(send_data);
	}

	void SetProcPpacket(std::function<void(std::shared_ptr<Packet>)>&& _proc_packet)
	{
		m_proc_packet = std::move(_proc_packet);
	}

protected:
	void ProcPacket(std::shared_ptr<NetPacket> _packet)
	{
		if (nullptr == _packet || nullptr == m_proc_packet)
		{
			return;
		}

		m_proc_packet(_packet);
	}

protected:
	eServerType m_session_type = eServerType_None;
	std::function<void(std::shared_ptr<Packet>)> m_proc_packet = nullptr;
};