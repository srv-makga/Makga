#pragma once

#include "server.h"

class ServerIOCP : public IServer
{
public:
	ServerIOCP();
	virtual ~ServerIOCP();

	bool Start() override;
	bool Wait() override;
	bool End() override;

public:
	bool Initialize();
	bool Finalize();
};