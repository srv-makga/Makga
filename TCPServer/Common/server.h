#pragma once

class IServer
{
public:
	IServer() = default;
	virtual ~IServer() = default;

	virtual bool Start() = 0;
	virtual bool Wait() = 0;
	virtual bool End() = 0;
};