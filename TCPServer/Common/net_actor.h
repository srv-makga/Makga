#pragma once

#include "common_type.h"

class NetHandler;
class JobHandler;
class SessionBase;

/*
* @brief AcceptorBase, Connector�� �������̽�
*/

class NetActor
{
protected:
	/*
	* @detail ��Ʈ��ũ �̺�Ʈ�� ���� AcceptorBase�� �Լ��� ȣ�����ش�
	* Select, IOCP �� ������ �ִ�
	*/
	std::shared_ptr<NetHandler> m_net_handler;
	/*
	* @detail logic thread
	*/
	std::shared_ptr<JobHandler> m_job_handler;

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

	virtual void InitHandler(std::shared_ptr<NetHandler> _net_handler, std::shared_ptr<JobHandler> _job_handler)
	{
		m_net_handler = _net_handler;
		m_job_handler = _job_handler;
	}

	virtual bool OnAccepted(SessionBase*, IOContext_t*) = 0;
	virtual bool OnConnected(SessionBase*, IOContext_t*) = 0;
	virtual bool OnReceived(SessionBase*, DWORD, IOContext_t*) = 0;
	virtual bool OnSent(SessionBase*, DWORD, IOContext_t*) = 0;
	virtual bool OnClosed(SessionBase*, IOContext_t*) = 0;

	virtual std::shared_ptr<NetHandler> NetHandler() const { return m_net_handler; }
	virtual std::shared_ptr<JobHandler> JobHandler() const { return m_job_handler; }
};