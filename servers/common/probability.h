#pragma once

#include <random>
#include <concepts>
#include <type_traits>
#include <ranges>
#include <unordered_map>

struct ItemProbability
{
	Prob prob;
	ItemIdx item_idx;
	StackCount min_count;
	StackCount max_count;
};

template<typename T>
concept ProbabilityConcept = std::ranges::range<T> &&
std::same_as<std::remove_cv_t<std::ranges::range_value_t<T>>, ItemProbability>&&
	requires(const T& t) { t.begin(); t.end(); t.empty(); t.size(); };

class ProbabilitySimulator
{
public:
	ProbabilitySimulator();
	~ProbabilitySimulator();

	// @brief 아이템 목록에서 n번 시도시 확률에 따른 획득 기대 시뮬레이션
	// @detail T가 Probability를 원소로 갖는 컨테이너임을 컴파일 타임에 강제
	template<ProbabilityConcept T>
	bool Run(const T& table, int try_count, OUT std::unordered_map<ItemIdx, StackCount>& result, Prob total_prob = max_prob)
	{
		if (table.size() >= static_cast<size_t>(try_count))
		{
			return CalculateSparse(table, try_count, result, total_prob);
		}
		
		return CalculateDense(table, try_count, result, total_prob);
	}

protected:
	// @brief 테이블 갯수보다 시도 횟수가 적을 때
	template<ProbabilityConcept T>
	bool CalculateSparse(const T& table, int try_count, OUT std::unordered_map<ItemIdx, StackCount>& result, Prob total_prob);

	// @brief 테이블 갯수보다 시도 횟수가 많을 때
	template<ProbabilityConcept T>
	bool CalculateDense(const T& table, int try_count, OUT std::unordered_map<ItemIdx, StackCount>& result, Prob total_prob);

protected:
	std::mt19937 rng_;
};

template<ProbabilityConcept T>
inline bool ProbabilitySimulator::CalculateSparse(const T& table, int try_count, OUT std::unordered_map<ItemIdx, StackCount>& result, Prob total_prob)
{
	for (int i = 0; i < try_count; ++i)
	{
		std::uniform_int_distribution dist(0, total_prob);
		Prob roll = dist(rng_);
		Prob accumulated_prob = 0;

		for (const auto& info : table)
		{
			accumulated_prob += info.prob;
			if (roll > accumulated_prob)
			{
				continue;
			}
			
			StackCount count = info.min_count;
			if (info.min_count < info.max_count)
			{
				count += (rng_() % (info.max_count - info.min_count + 1));
			}
			result[info.item_idx] += count;
			break;
		}
	}

	return !result.empty();
}

template<ProbabilityConcept T>
inline bool ProbabilitySimulator::CalculateDense(const T& table, int try_count, OUT std::unordered_map<ItemIdx, StackCount>& result, Prob total_prob)
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
		if (info.min_count < info.max_count)
		{
			count += (rng_() % (info.max_count - info.min_count + 1));
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
