#include "pch.h"
#include "ranking_content.h"

static constexpr int64_t s_redis_max_number = 9007199254740992;

RankingContent::RankingContent(const std::string& key, int32_t top_count)
	: redis_key_(key)
	, top_count_(top_count)
	, next_update_time_(std::numeric_limits<std::time_t>::max())
{
}

void RankingContent::AddScore(uint64_t member, int64_t score)
{
	score = std::min(score, s_redis_max_number);

	if (0 == score)
	{
		sendData(std::format("ZREM {0}:ranking {1}", redis_key_, member));
	}
	else
	{
		sendData(std::format("ZADD {0}:ranking {1} {2}", redis_key_, score, member));
	}
}

void RankingContent::CalcRanking()
{
	if (0 == top_count_)
	{
		return;
	}

	if (GetNextUpdateTime() > std::time(nullptr))
	{
		return;
	}

	SetNextUpdateTime(CalcNextUpdateTime());

	// @todo redis connector implement
}

void RankingContent::Backup(std::time_t ttl)
{
	// @todo redis connector implement
}

void RankingContent::BackupTop(std::time_t ttl)
{
	// @todo redis connector implement
}

const std::string& RankingContent::GetRedisKey() const
{
	return redis_key_;
}

std::time_t RankingContent::GetNextUpdateTime() const
{
	return next_update_time_;
}

void RankingContent::SetNextUpdateTime(std::time_t time)
{
	next_update_time_ = time;
}

void RankingContent::SendData(std::string&& _data)
{
	// @todo redis connector implement
}

void RankingContent::SendTopRanking(std::string&& _data)
{
	// @todo redis connector implement
}