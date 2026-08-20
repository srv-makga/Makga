#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <limits>
#include "../../3rdparty/hiredis/hiredis.h"

import <string>;
import makga.lib.database.redis.connector;

// @brief Ranking content interface
// @detail Register in RANKING_SYSTEM and use it.
class RankingContent
{
public:
	enum class StorageStatus : std::uint8_t
	{
		NotConfigured,
	};
	// @param key: Key name to use in redis
	// @param top_count: The number of top rankings to fetch from redis
	RankingContent(const std::string& key, int32_t top_count);
	virtual ~RankingContent() = default;

	virtual void LoadData() = 0;

	// @brief Function called when top ranking data is fetched from redis
	// @detail Use member and score separately in reply
	virtual void ProcRanking(std::shared_ptr<redisReply> reply) = 0;

	virtual std::time_t CalcNextUpdateTime() const = 0;

	void AddScore(uint64_t member, int64_t score);
	void CalcRanking();

	void Backup(std::time_t ttl = 0);
	void BackupTop(std::time_t ttl = 0);

	const std::string& GetRedisKey() const;
	StorageStatus GetStorageStatus() const noexcept;
	bool IsStorageConfigured() const noexcept;
	std::uint64_t GetRejectedStorageOperationCount() const noexcept;
	std::time_t GetNextUpdateTime() const;
	void SetNextUpdateTime(std::time_t time);

protected:
	void SendCommand(std::string&& _data);
	void SendTopRanking(std::string&& _data);
	void RecordStorageUnavailable() noexcept;

protected:
	const std::string redis_key_;
	int32_t top_count_ = 0;
	std::time_t next_update_time_ = 0;
	std::atomic<std::uint64_t> rejected_storage_operations_{0};
};

// @brief Mock class to prevent nullptr
class RankingContentMock : public RankingContent
{
public:
	RankingContentMock()
		: RankingContent("RankingContentMock", 0) {
	}
	virtual ~RankingContentMock() = default;

	void LoadData() override {}
	void ProcRanking(std::shared_ptr<redisReply> reply) override { (void)reply; }
	std::time_t CalcNextUpdateTime() const override { return std::numeric_limits<std::time_t>::max(); }
	
	void AddScore(uint64_t member, int64_t score) { RankingContent::AddScore(member, score); }
	void CalcRanking() { RankingContent::CalcRanking(); }
};
