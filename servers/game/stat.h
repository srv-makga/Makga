#pragma once

class Actor;
class Stat
{
	using Key = uint64_t;
	using Record = std::pair<makga::StatType, EffectValue>;

public:
	Stat(std::shared_ptr<Actor> owner);
	virtual ~Stat();

	bool Initialize();
	void Finalize();

	void CopyTo(std::shared_ptr<Stat> other) const;

	EffectValue GetStat(makga::StatType stat_type) const;
	Key AddStat(makga::StatType stat_type, EffectValue value);
	void DelStat(Key key);

protected:
	std::shared_ptr<Actor> owner_;
	Key next_key_;

	std::unordered_map<Key, Record> records_;
	std::unordered_map<makga::StatType, EffectValue> stats_;
};