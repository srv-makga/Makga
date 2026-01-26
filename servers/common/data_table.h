#pragma once

#include "data_struct.h"

template<typename T>
class Table
{
public:
	using Container = std::map<TableIdx, T>;
	using Iterator = typename Container::iterator;
	using ConstIterator = typename Container::const_iterator;

	Table() = default;
	~Table() = default;

	// 조회: 포인터 반환 (없으면 nullptr)
	std::optional<const T*> TryGet(TableIdx idx) const
	{
		auto it = data_.find(idx);
		if (it == data_.end()) return std::nullopt;
		return &it->second;
	}

	T& Get(TableIdx idx)
	{
		return data_[idx];
	}

	// emplace: 새 항목 추가 (존재하면 추가 실패)
	std::pair<Iterator, bool> Emplace(TableIdx idx, T value)
	{
		return data_.emplace(idx, std::move(value));
	}

	// insert_or_assign: 항상 삽입 또는 교체
	void Insert(TableIdx idx, T value)
	{
		data_.insert_or_assign(idx, std::move(value));
	}

	// 존재	여부 확인
	bool Contains(TableIdx idx) const
	{
		return data_.find(idx) != data_.end();
	}

	bool Erase(TableIdx idx)
	{
		return 0 < data_.erase(idx);
	}

	// 반복자/범위 기반 for 지원
	Iterator begin() { return data_.begin(); }
	Iterator end() { return data_.end(); }
	ConstIterator begin() const { return data_.begin(); }
	ConstIterator end() const { return data_.end(); }
	ConstIterator cbegin() const { return data_.cbegin(); }
	ConstIterator cend() const { return data_.cend(); }

	//template<typename F>
	//void ForEach(F&& f) const
	//{
	//	for (const auto& p : data_)
	//	{
	//		std::invoke(std::forward<F>(f), p.first, p.second);
	//	}
	//}

	void Clear()
	{
		data_.clear();
	}

	std::size_t Size() const
	{
		return data_.size();
	}

	bool Empty() const
	{
		return data_.empty();
	}

private:
	Container data_;
};

class DataTable
{
public:
	// 주로 key - value 쌍으로 이루어진 데이터를 다룸
	DataTable() = default;
	~DataTable() = default;

	// 타입 별 편의 alias
	using MapTableTable = Table<MapTable>;
	using MonsterTableTable = Table<MonsterTable>;

	// 멤버 테이블 (공개하여 직접 접근/조작 가능)
	MapTableTable map_table_;
	MonsterTableTable monster_table_;
};