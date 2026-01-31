#pragma once

#include <random>
#include <concepts>
#include <type_traits>
#include <ranges>
#include <unordered_map>

struct Probability
{
	Prob prob;
	ItemIdx item_idx;
	StackCount min_count;
	StackCount max_count;
};

class ProbabilitySimulator
{
public:
	ProbabilitySimulator();
	~ProbabilitySimulator();

	// T가 Probability를 원소로 갖는 컨테이너임을 컴파일 타임에 강제
	template<typename T>
	requires
		std::ranges::range<T> &&
		std::same_as<std::remove_cv_t<std::ranges::range_value_t<T>>, Probability> &&
		requires(const T& t) { t.empty(); t.size(); }
	bool Run(const T& table, int try_count, OUT std::unordered_map<ItemIdx, StackCount>& result, Prob total_prob = max_prob)
	{
		result.clear();

		if (true == table.empty())
		{
			return false;
		}

		if (0 >= try_count)
		{
			return false;
		}

		Prob accumulated_prob = 0;
		for (const auto& info : table)
		{
			accumulated_prob += info.prob;
		}

		if (accumulated_prob != total_prob)
		{
			return false;
		}

		int remaining_tries = try_count;
		int table_size = static_cast<int>(table.size());
		int i = 0;

		for (const auto& info : table)
		{
			if (0 >= remaining_tries)
			{
				break;
			}

			StackCount count = info.min_count;
			if (info.min_count != info.max_count)
			{
				count = info.min_count + (rng_() % (info.max_count - info.min_count + 1));
			}

			// 마지막 항목은 남은 시도 횟수를 모두 할당
			if (i == table_size - 1)
			{
				result[info.item_idx] += (remaining_tries * count);
				break;
			}

			double prob_ratio = static_cast<double>(info.prob) / static_cast<double>(total_prob - accumulated_prob);
			if (prob_ratio > 1.0)
			{
				prob_ratio = 1.0;
			}

			std::binomial_distribution dist(remaining_tries, prob_ratio);
			StackCount trials = dist(rng_);

			result[info.item_idx] += (trials * count);

			remaining_tries -= trials;
			accumulated_prob += info.prob;

			++i;
		}

		return !result.empty();
	}

private:
	std::mt19937 rng_;
};