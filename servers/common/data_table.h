#pragma once

#include "data_struct.h"

import makga.lib.pattern.singleton;

template<typename Key, typename Value>
class Table
{
public:
	using Container = std::map<Key, Value>;
	using Iterator = typename Container::iterator;
	using ConstIterator = typename Container::const_iterator;

	Table() = default;
	~Table() = default;

	// 조회: 포인터 반환 (없으면 nullptr)
	std::optional<const Value*> TryGet(Key key) const
	{
		auto it = data_.find(key);
		if (it == data_.end()) return std::nullopt;
		return &it->second;
	}

	Value& Get(Key key)
	{
		return data_[key];
	}

	// emplace: 새 항목 추가 (존재하면 추가 실패)
	std::pair<Iterator, bool> Emplace(Key key, Value value)
	{
		return data_.emplace(key, std::move(value));
	}

	// insert_or_assign: 항상 삽입 또는 교체
	void Insert(Key key, Value value)
	{
		data_.insert_or_assign(key, std::move(value));
	}

	// 존재	여부 확인
	bool Contains(Key key) const
	{
		return data_.find(key) != data_.end();
	}

	bool Erase(Key key)
	{
		return 0 < data_.erase(key);
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

class DataTable : public makga::lib::Singleton<DataTable>
{
public:
	using MapTableTable = Table<TerrainIdx, MapTable>;
	using MonsterTableTable = Table<MonsterIdx, MonsterTable>;
	using SkillTableTable = Table<SkillIdx, SkillTable>;
	using SpawnGroupTableTable = Table<TableIdx, SpawnGroupTable>;

	DataTable() = default;
	virtual ~DataTable() = default;

	virtual bool LoadTable();

public:
	bool DataVerification();
	bool LoadFileToBuffer(const std::string& file_path, rapidjson::Document& doc);

	bool LoadMapTable(const std::string& file_path);
	bool LoadMonsterTable(const std::string& file_path);
	bool LoadSkillTable(const std::string& file_path);

	bool LoadSpawnGroupTable(const std::string& file_path);
	std::optional<const SpawnGroupTable*> TryGetSpawnGroupTable(TableIdx idx) const
	{
		return spawn_group_table_.TryGet(idx);
	}

protected:
	std::vector<char> file_buffer_;

	MapTableTable map_table_;
	MonsterTableTable monster_table_;
	SkillTableTable skill_table_;
	SpawnGroupTableTable spawn_group_table_;
};