#pragma once

#include "common_header.h"

class Service
{
public:
	Service() = default;
	virtual ~Service() = default;

	virtual bool Start(std::function<void(void)> _func) = 0;
	virtual bool Stop() = 0;
};