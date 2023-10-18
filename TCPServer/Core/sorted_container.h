#pragma once

#include "core_type.h"
#include <set>
#include <algorithm>

namespace core {
namespace ranking {
template<typename T>
concept RankingElement_t = std::is_pointer_v<T> || std::is_integral_v<T>;

template<RankingElement_t T>
struct RankingLess
{
	bool operator()(const T& p1, const T& p2) const
	{
		return p1.score < p2.score;  // Compare based on score
	}
};

template<RankingElement_t T>
struct RankingGreater
{
	bool operator()(const T& p1, const T& p2) const
	{
		return p1.score > p2.score;  // Compare based on score
	}
};

template<typename Score_t, RankingElement_t Element_t, typename OrderType_t = RankingGreater<Element_t>>
class SortedContainer
{
public:
	SortedContainer()
	{

	}

	virtual ~SortedContainer()
	{

	}

	void Add(Element_t _element)
	{
		m_sorted.insert(_element);
	}

	void Update(Element_t _element)
	{
		remove(_element);
		add(_element);
	}

	void remove(Element_t _element)
	{
		auto iter = std::find_if(m_sorted.begin(), m_sorted.end(), [_element](const Element_t& _iter) {
			return _iter = _element;
		});

		if (m_sorted.end() != iter)
		{
			m_sorted.erase(iter);
		}
	}

private:
	std::set<Element_t, OrderType_t> m_sorted;
};
} // namespace ranking
} // namespace core
