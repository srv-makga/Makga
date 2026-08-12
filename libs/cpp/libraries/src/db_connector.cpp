module;

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

module makga.lib.database.connector;

import makga.lib.logger;

namespace makga::lib::database {

DbConnector::DbConnector() = default;

DbConnector::~DbConnector()
{
	Finalize();
}

bool DbConnector::Initialize(const DbConfig& config, Factory factory)
{
	config_  = config;
	running_ = true;

	for (int32_t i = 0; i < config.pool_size; ++i)
	{
		auto conn = factory();
		if (!conn->Connect(config))
		{
			MakgaLogger::Error() << "DbConnector: connection " << i << " failed.";
			return false;
		}
		free_pool_.push(conn.get());
		pool_.push_back(std::move(conn));
	}

	// worker thread 수 = pool_size / 2, 최소 1개
	int32_t worker_count = std::max(1, config.pool_size / 2);
	for (int32_t i = 0; i < worker_count; ++i)
		workers_.emplace_back(&DbConnector::WorkerLoop, this);

	return true;
}

void DbConnector::Finalize()
{
	running_ = false;
	task_cv_.notify_all();

	for (auto& w : workers_)
		if (w.joinable()) w.join();
	workers_.clear();

	std::unique_lock lock(pool_mutex_);
	for (auto& conn : pool_)
		conn->Disconnect();
	pool_.clear();
	while (!free_pool_.empty()) free_pool_.pop();
}

IDbConnector* DbConnector::AcquireConnection()
{
	std::unique_lock lock(pool_mutex_);
	pool_cv_.wait(lock, [this] { return !free_pool_.empty(); });
	auto* conn = free_pool_.front();
	free_pool_.pop();
	return conn;
}

void DbConnector::ReleaseConnection(IDbConnector* conn)
{
	{
		std::unique_lock lock(pool_mutex_);
		free_pool_.push(conn);
	}
	pool_cv_.notify_one();
}

bool DbConnector::Execute(const std::string& query, DbRows& out_rows)
{
	auto* conn = AcquireConnection();
	bool  ok   = conn->Execute(query, out_rows);
	ReleaseConnection(conn);
	return ok;
}

bool DbConnector::ExecuteNonQuery(const std::string& query)
{
	auto* conn = AcquireConnection();
	bool  ok   = conn->ExecuteNonQuery(query);
	ReleaseConnection(conn);
	return ok;
}

void DbConnector::ExecuteAsync(const std::string& query, DbCallback callback)
{
	{
		std::unique_lock lock(task_mutex_);
		task_queue_.push({ query, std::move(callback) });
	}
	task_cv_.notify_one();
}

void DbConnector::WorkerLoop()
{
	while (true)
	{
		Task task;
		{
			std::unique_lock lock(task_mutex_);
			task_cv_.wait(lock, [this]
			{
				return !task_queue_.empty() || !running_;
			});

			if (!running_ && task_queue_.empty())
				break;

			task = std::move(task_queue_.front());
			task_queue_.pop();
		}

		DbRows rows;
		bool   ok = Execute(task.query, rows);
		if (task.callback)
			task.callback(ok, std::move(rows));
	}
}

} // namespace makga::lib::database
