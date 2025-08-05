#pragma once

#include "game_header.h"
#include "app_config.h"
#include "../Common/job_handler.h"

class AppSession : public Session_t, public JobOwner
{
public:
	AppSession(core::ServiceType _type, std::size_t _buffer_size, eServerType _session_type);
	virtual ~AppSession();

	void Send(PacketId_t _pid, FB_BUILDER& _fbb);

public: // Session_t
	void ProcConnect() override;
	virtual void ProcDisconnect() override;
	virtual std::size_t ProcRecv(char* _data, std::size_t _data_size) override;
	virtual void ProcSend(std::size_t) override;

protected:
	eServerType m_session_type;
};