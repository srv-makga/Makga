#pragma once

#include <unordered_map>

template<typename Key_t, typename Val_t>
class ItemContainerBase
{
public:
	using Container_t = std::unordered_map<Key_t, Val_t>;

public:
	ItemContainerBase() = default;
	~ItemContainerBase() = default;

	void Initialize()
	{
		m_items.clear();
	}

	bool Insert(Key_t _key, Val_t _val)
	{
		return m_items.insert({ _key, _val }).second;
	}

	bool Erase(Key_t _key)
	{
		return 0 < m_items.erase(_key);
	}

	Val_t Find(Key_t _key) const
	{
		auto iter = m_items.find(_key);
		if (m_items.end() == iter)
		{
			return Val_t();
		}

		return iter->second;
	}

	bool Has(Key_t _key) const
	{
		return Val_t() != Find(_key);
	}

	Val_t Fetch(Key_t _key)
	{
		auto iter = m_items.find(_key);
		if (m_items.end() == iter)
		{
			return Val_t();
		}

		Val_t val = iter->second;

		m_items.erase(_key);
		return val;
	}

	void FetchAll(std::vector<Val_t>& _vals)
	{
		m_items.reserve(m_items.size() + _vals.size());

		for (auto& iter : m_items)
		{
			_vals.push_back(iter.second);
		}
		
		m_items.clear();
	}

	std::size_t Size() const
	{
		return m_items.size();
	}
	
	auto Begin() { return m_items.begin(); }
	auto End() { return m_items.end(); }
	auto Begin() const { return m_items.begin(); }
	auto End() const { return m_items.end(); }

private:
	Container_t m_items;
};