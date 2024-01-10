#pragma once

#include "../Core/singleton.hpp"
#include "common_header.h"
#include <random>

class Utility : public core::pattern::Singleton<Utility>
{
public:
	Utility();
	~Utility() = default;

	bool Readfile(const char* _file_name, char* _buffer, std::size_t _buffer_size);
	bool Tokenization(const std::string& _data, const char _token, std::vector<std::string>& _output);

	// Position
	bool IsSame(const PositionT& _pos1, const PositionT& _pos2);
	Coord_t CalcDistance(const PositionT& _pos1, const PositionT& _pos2);

	// time
	Time_t CurrentTime();
	Tick_t CurrentTick();

	template <typename T>
	typename std::enable_if_t<std::is_integral<T>::value, T> Random(T _min = std::numeric_limits<T>::min(), T _max = std::numeric_limits<T>::max())
	{
		std::uniform_int_distribution<T> distribution(_min, _max);
		return distribution(m_random_engine);
	}

	template <typename T>
	typename std::enable_if_t<std::is_floating_point<T>::value, T> Random(T _min = std::numeric_limits<T>::min(), T _max = std::numeric_limits<T>::max())
	{
		std::uniform_real_distribution<T> distribution(_min, _max);
		return distribution(m_random_engine);
	}

private:
	std::random_device m_rd;
	std::mt19937 m_random_engine;
};

#define UTIL	Utility::Instance()