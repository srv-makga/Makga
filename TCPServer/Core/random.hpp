#pragma once

#include <random>

namespace core {
template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

class RandomEngine
{
	std::mt19937 m_engine;

public:
	RandomEngine()
	{
		uint32_t seed = static_cast<uint32_t>(time(nullptr));
		m_engine.seed(seed);
	}

	template<arithmetic T>
	T Rand(T _start, T _end)
	{
		if (_start == _end)
		{
			return _start;
		}

		std::uniform_int_distribution<T> distribution(start, end);
		return distribution(m_engine);
	}

	template<>
	float Rand<float>(float _start, float _end)
	{
		if (_start == _end)
		{
			return _start;
		}

		std::uniform_real_distribution<float> distribution(_start, _end);
		return distribution(m_engine);
	}

	template<>
	double Rand<double>(double _start, double _end)
	{
		if (_start == _end)
		{
			return _start;
		}

		std::uniform_real_distribution<double> distribution(_start, _end);
		return distribution(m_engine);
	}
};
} // namespace core