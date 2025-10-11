#pragma once

#include "security_interface.h"

class SecurityDummy : public SecurityInterface
{
public:
	SecurityDummy() = default;
	virtual ~SecurityDummy() = default;

	bool Initialize() override { return true; }
	void Finalize() override {}

	eResult Register(User* _user) override { return eResult::eResult_Success; }
};
