#pragma once

#include "game_header.h"

enum class SecurityType
{
	None = 0,
	NProject,
	Xigncode,
	Xtrap,
};

class SecurityInterface;
class SecuritySystem
{
public:
	SecuritySystem() = default;
	virtual ~SecuritySystem() = default;

public:
	static std::shared_ptr<SecurityInterface> Create(SecurityType _type);
};