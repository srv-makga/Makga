#pragma once

#include "game_header.h"

class User;
class SecurityInterface
{
public:
	SecurityInterface() = default;
	virtual ~SecurityInterface() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;

	virtual eResult Register(User* _user) = 0;
};