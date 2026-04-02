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

	// @brief 확률 시뮬레이터. 아이템 드랍, 가챠 등 확률 기반 결과 생성에 사용.
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
	// @brief 확률 테이블이 try_count보다 크거나 같은 경우, 각 시도마다 독립적으로 확률을 굴려 결과를 누적하는 방식
	template<ProbabilityConcept T>
	bool CalculateSparse(const T& table, int try_count, OUT std::unordered_map<ItemIdx, StackCount>& result, Prob total_prob);

	// @brief 확률 테이블이 try_count보다 작은 경우, 전체 시도에 대한 결과를 한 번에 계산하는 방식 (예: 이항 분포 활용)
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

		// 마지막 항목인 경우, 남은 시도 전체를 해당 항목으로 처리 (확률이 100%이므로)
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