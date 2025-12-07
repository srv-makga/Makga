module;

#include <random>

export module makga.lib.random;

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

export namespace makga::lib {
export class RandomEngine
{
public:
	RandomEngine()
	{
		uint32_t seed = static_cast<uint32_t>(time(nullptr));
		engine_.seed(seed);
	}

	template<Arithmetic T>
	T Rand(T start, T end)
	{
		if (start == end)
		{
			return start;
		}

		std::uniform_int_distribution<T> distribution(start, end);
		return distribution(engine_);
	}

	template<>
	float Rand<float>(float start, float end)
	{
		if (start == end)
		{
			return start;
		}

		std::uniform_real_distribution<float> distribution(start, end);
		return distribution(engine_);
	}

	template<>
	double Rand<double>(double start, double end)
	{
		if (start == end)
		{
			return start;
		}

		std::uniform_real_distribution<double> distribution(start, end);
		return distribution(engine_);
	}

private:
	std::mt19937 engine_;
};
} // namespace makga::lib