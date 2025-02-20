#pragma once

#include "security.h"

class SecurityXigncode : public SecuritySystem
{
public:
	SecurityXigncode() = default;
	virtual ~SecurityXigncode() = default;

public:
	bool Initialize() override;
	void Finalize() override;
	eResult Register(User* _user) override;
	void ReceiveXigncodePacket(User* _user, char* _data);

private:
	/*xbool XCALL SendProc(xpvoid ptr, xpvoid meta, xpcch buf, xulong size);
	void XCALL DisconnectProc(xpvoid uid, xpvoid meta, xint code, xctstr rereport);*/

private:
	//IXigncodeServerPtr m_server;
	const int m_xigncode_packet_size = 512;
};