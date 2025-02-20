#pragma once

#include "security.h"

class SecurityNprotect : public SecuritySystem
{
public:
	SecurityNprotect() = default;
	virtual ~SecurityNprotect() = default;

public:
	bool Initialize() override;
	void Finalize() override;
	eResult Register(User* _user) override;
};