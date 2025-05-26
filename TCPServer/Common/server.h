#pragma once

#include "../Core/lock.h"

class Server
{
public:
	Server() = default;
	~Server() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;

	bool Start()
	{
		if (false == StartUp())
		{
			return false;
		}

		if (false == StartUpEnd())
		{
			return false;
		}

		return true;
	}

protected:
	virtual bool StartUp() = 0;
	virtual bool StartUpEnd() = 0;
};