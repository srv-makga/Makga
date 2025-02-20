#pragma once

#include "security.h"

class SecurityXtrap : public SecuritySystem
{
public:
	SecurityXtrap() = default;
	virtual ~SecurityXtrap() = default;

public:
	bool Initialize() override;
	void Finalize() override;
	eResult Register(User* _user) override;
};