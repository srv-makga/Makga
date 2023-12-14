#pragma once

#include "../Core/singleton.hpp"
#include "common_header.h"

class Utility : public core::pattern::Singleton<Utility>
{
public:
	Utility() = default;
	~Utility() = default;

	bool Readfile(const char* _file_name, char* _buffer, std::size_t _buffer_size);
	bool Tokenization(const std::string& _data, const char _token, std::vector<std::string>& _output);

	// Position
	Coord_t CalcDistance(const PositionT& _pos1, const PositionT& _pos2);

	// time
	Time_t CurrentTime();
};

#define UTIL	Utility::Instance()