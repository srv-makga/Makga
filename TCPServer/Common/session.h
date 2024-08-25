#pragma once

#include "common_type.h"
#include "../Core/ip_endpoint.h"

class ISession
{
public:
	ISession() = default;
	virtual ~ISession() = default;

	virtual bool Initialize() = 0;
	virtual void FInalize() = 0;

public:
	virtual bool PostAccept(SOCKET _socket) = 0;
	virtual bool PostConnect(const core::network::IPEndPoint& _endpoint) = 0;
	virtual bool PostDisconnect() = 0;
	virtual bool PostZeroRecv() = 0;
	virtual bool PostRecv() = 0;
	virtual bool PostSend() = 0;

	virtual void OnAccept(IOContext_t* _context) = 0;
	virtual void OnConnect() = 0;
	virtual void OnDisconnect() = 0;
	virtual void OnRecv(DWORD _bytes_transferred, IOContext_t* _context) = 0;
	virtual void OnSent(DWORD _bytes_transferred) = 0;

public:
	virtual SessionId_t SessionId() const { return SessionId_t(); }
};