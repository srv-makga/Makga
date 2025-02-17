#pragma once

#include "game_header.h"

enum class SecurityType
{
	None = 0,
	Xtrap,
	Xigncode,
};

class User;
class SecuritySystem
{
public:
	SecuritySystem() = default;
	virtual ~SecuritySystem() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;

	virtual eResult Register(User* _user) = 0;

public:
	static std::shared_ptr<SecuritySystem> Create(SecurityType _type);
};