#pragma once

#include "security_interface.h"

class SecurityXtrap : public SecurityInterface
{
public:
	SecurityXtrap() = default;
	virtual ~SecurityXtrap() = default;

public:
	bool Initialize() override;
	void Finalize() override;
	eResult Register(User* _user) override;
};