#pragma once

#include <memory>

import <unordered_map>;
import <string>;

class RankingContent;
class RankingSystem
{
public:
	using Key = std::string;

public:
	RankingSystem();
	virtual ~RankingSystem();

	bool Initialize();
	void Finalize();

	bool AddContent(const Key& key, std::shared_ptr<RankingContent> content);
	std::shared_ptr<RankingContent> GetContent(const Key& key);
	bool RemoveContent(const Key& key);

	void Update();

protected:
	std::unordered_map<Key, std::shared_ptr<RankingContent>> ranking_contents_;
};