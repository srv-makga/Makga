#pragma once

#include "common_type.h"
#include "config.h"
#include "net_actor.h"
#include "session_base.h"

#include "../Core/lock.h"
#include "../Core/socket_tcp.h"
#include "../Core/io_context.hpp"

#include <unordered_set>
#include <atomic>

class JobHandler;
class NetHandler;

/*
* @brief ���� ���Ӱ� ���� ����, �̺�Ʈ �߻��� ó�� ���� ��� Ŭ����
* @detail ���� ������ �ް�, ��Ʈ��ũ �̺�Ʈ�� �޾� ��ũ ��������� ����
*/
class AcceptorBase : public NetActor
{
public:
	AcceptorBase();
	virtual ~AcceptorBase();

	virtual void Initialize() override;
	void Finalize() override;

	void InitSetting(int32_t _accept_max_count);
	void InitSocket(const std::string& _ipaddr, Port_t _port);
	virtual void InitSession(std::size_t _max_session) = 0;

	bool OnAccepted(std::shared_ptr<SessionBase> _session, IOContext_t* _context) override;
	bool OnConnected(std::shared_ptr<SessionBase>, IOContext_t* _context) override { return false; }
	bool OnReceived(std::shared_ptr<SessionBase> _session, DWORD _bytes_transferred, IOContext_t* _context) override;
	bool OnSent(std::shared_ptr<SessionBase> _session, DWORD _bytes_transferred, IOContext_t* _context) override;
	bool OnClosed(std::shared_ptr<SessionBase> _session, IOContext_t* _context) override;

protected:
	bool AddSession(std::shared_ptr<SessionBase> _session);
	void RemoveSession(std::shared_ptr<SessionBase> _session);
	std::shared_ptr<SessionBase> FindSession(SessionId_t _sessionId);

	void AddWaitSession(std::shared_ptr<SessionBase> _session);
	void RemoveWaitSession(std::shared_ptr<SessionBase> _session);

	virtual std::shared_ptr<SessionBase> AllocSession() = 0;
	virtual void ReallocSession(std::shared_ptr<SessionBase> _session) = 0;

private:
	bool RegisterAcceptex(std::shared_ptr<SessionBase> _session);
	bool RegisterAcceptex();

	/*
	* @brief ���� ���� ��������
	*/
	bool IsConnected() const;

public:
	fb::eServerType ServerType() const;
	void SetServerType(fb::eServerType _server_type);

	std::size_t MaxAcceptSize() const;
	void SetMaxAcceptSize(std::size_t _accept_max_size);
	std::size_t UsingAcceptSize() const;
	void SetUsingAcceptSize(std::size_t _accept_use_size);

protected:
	std::unordered_map<SessionId_t, std::shared_ptr<SessionBase>> m_using_sessions; // ���� �Ϸ�
	core::RCMutex m_using_mutex;

	std::unordered_set<std::shared_ptr<SessionBase>> m_wait_sessions; // accept �ɾ��
	core::RCMutex m_wait_mutex;

	std::queue<std::shared_ptr<SessionBase>> m_free_sessions;
	core::RCMutex m_free_mutex;

private:
	core::network::SocketTcp m_socket;

	fb::eServerType m_server_type;

	// @brief Accept �ִ� ���� ����
	std::atomic<std::size_t> m_max_accept_size;
	// @brief ���� ���� ���� ���� ����
	std::atomic<std::size_t> m_use_accept_size;
	// @brief  ���� ���� ����
	std::atomic<bool> m_is_connect;
};