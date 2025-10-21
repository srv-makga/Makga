#pragma once

#include "security_interface.h"

class SecurityNprotect : public SecurityInterface
{
public:
	SecurityNprotect() = default;
	virtual ~SecurityNprotect() = default;

public:
	bool Initialize() override;
	void Finalize() override;
	eResult Register(User* _user) override;
};