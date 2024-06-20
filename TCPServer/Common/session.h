#pragma once

#include "common_type.h"

class ISession
{
public:
	ISession() = default;
	virtual ~ISession() = default;

	virtual bool Initialize() = 0;
	virtual void FInalize() = 0;

	virtual SessionId_t SessionId() const { return SessionId_t(); }
};