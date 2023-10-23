#pragma once

#include "../Core/singleton.hpp"

class Function final : public core::pattern::Singleton<Function>
{
public:
	Function() = default;
	~Function() = default;
};


#define Function Function::Instance()