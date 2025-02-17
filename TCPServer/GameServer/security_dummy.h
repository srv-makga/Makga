#pragma once

#include "security.h"

class SecurityDummy : public SecuritySystem
{
public:
	SecurityDummy() = default;
	virtual ~SecurityDummy() = default;

	bool Initialize() override { return true; }
	void Finalize() override {}

	eResult Register(User* _user) override { return eResult::Success; }
};
