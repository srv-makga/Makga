#include "stdafx.h"
#include "ranking_system.h"
#include "ranking_content.h"

import <unordered_map>;
import <string>;

RankingSystem::RankingSystem()
{
}

RankingSystem::~RankingSystem()
{
	ranking_contents_.clear();
}

bool RankingSystem::Initialize()
{
	return true;
}

void RankingSystem::Finalize()
{
	ranking_contents_.clear();
}

bool RankingSystem::AddContent(const Key& key, std::shared_ptr<RankingContent> content)
{
	return ranking_contents_.emplace(key, content).second;
}

std::shared_ptr<RankingContent> RankingSystem::GetContent(const Key& key)
{
	auto iter = ranking_contents_.find(key);
	if (ranking_contents_.end() == iter)
	{
		return nullptr;
	}

	return iter->second;
}

bool RankingSystem::RemoveContent(const Key& key)
{
	return 0 < ranking_contents_.erase(key);
}