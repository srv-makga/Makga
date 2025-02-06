#pragma once

#include "net_actor.h"

struct ConnectorConfig : public NetActorConfig
{

};

struct AcceptorConfig : public NetActorConfig
{
	std::size_t max_session;
};

// @brief Ư�� ������ �����ϱ� ���� Ŭ����
class Connector : public NetActor
{
public:
	Connector();
	virtual ~Connector();

public:
	bool Initialize() override;
	void Finalize() override;

private:
	bool OnAccepted(std::shared_ptr<Session_t>, IOContext_t*) override;
	bool OnConnected(std::shared_ptr<Session_t>, IOContext_t*) override;
	bool OnReceived(std::shared_ptr<Session_t>, DWORD, IOContext_t*) override;
	bool OnSent(std::shared_ptr<Session_t>, DWORD, IOContext_t*) override;
	bool OnClosed(std::shared_ptr<Session_t>, IOContext_t*) override;
};