#pragma once

#include "common_type.h"

class NetHandler;
class JobHandler;
class SessionBase;

/*
* @brief AcceptorBase, Connector의 인터페이스
*/

class NetActor
{
protected:
	/*
	* @detail 네트워크 이벤트에 따라 AcceptorBase의 함수를 호출해준다
	* Select, IOCP 등 종류가 있다
	*/
	NetHandler* m_net_handler;
	/*
	* @detail logic thread
	*/
	JobHandler* m_job_handler;

public:
	NetActor()
		: m_net_handler(nullptr)
		, m_job_handler(nullptr)
	{}
	virtual ~NetActor() {}

	virtual void Initialize()
	{
		m_net_handler = nullptr;
		m_job_handler = nullptr;
	}

	virtual void Finalize() = 0;

	virtual void InitHandler(NetHandler* _net_handler, JobHandler* _job_handler)
	{
		m_net_handler = _net_handler;
		m_job_handler = _job_handler;
	}

	virtual bool OnAccepted(SessionBase*, IOContext_t*) = 0;
	virtual bool OnConnected(SessionBase*, IOContext_t*) = 0;
	virtual bool OnReceived(SessionBase*, DWORD, IOContext_t*) = 0;
	virtual bool OnSent(SessionBase*, DWORD, IOContext_t*) = 0;
	virtual bool OnClosed(SessionBase*, IOContext_t*) = 0;

	virtual NetHandler* NetHandler() const { return m_net_handler; }
	virtual JobHandler* JobHandler() const { return m_job_handler; }
};