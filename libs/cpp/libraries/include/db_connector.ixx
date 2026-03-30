module;

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

export module makga.lib.database.connector;

export namespace makga::lib::database {

// ─── 연결 설정 ────────────────────────────────────────────────────────────────
export struct DbConfig
{
	std::string host      = "127.0.0.1";
	uint16_t    port      = 3306;
	std::string user;
	std::string password;
	std::string db_name;
	int32_t     pool_size = 4;
};

// ─── 쿼리 결과 타입 ───────────────────────────────────────────────────────────
// 한 행: { 컬럼명 → 값 }
export using DbRow      = std::unordered_map<std::string, std::string>;
export using DbRows     = std::vector<DbRow>;
// 비동기 완료 콜백 — 임의 thread에서 호출되므로 thread-safe 처리 필요
export using DbCallback = std::function<void(bool ok, DbRows rows)>;

// ─── 단일 커넥션 인터페이스 ───────────────────────────────────────────────────
export class IDbConnector
{
public:
	virtual ~IDbConnector() = default;

	virtual bool Connect(const DbConfig& config) = 0;
	virtual void Disconnect()                    = 0;
	virtual bool IsConnected() const             = 0;

	// SELECT — 결과를 out_rows에 채운다
	virtual bool Execute(const std::string& query, DbRows& out_rows) = 0;

	// INSERT / UPDATE / DELETE
	virtual bool ExecuteNonQuery(const std::string& query) = 0;
};

// ─── 커넥션 풀 + 비동기 쿼리 실행기 ─────────────────────────────────────────
//
//  사용 예)
//    DbConnector db;
//    db.Initialize(config, []{ return std::make_unique<MySqlConnector>(); });
//    db.ExecuteAsync("SELECT ...", [](bool ok, DbRows rows){ ... });
//
export class DbConnector
{
public:
	// IDbConnector 구현체를 생성하는 팩토리
	using Factory = std::function<std::unique_ptr<IDbConnector>()>;

	DbConnector();
	~DbConnector();

	// pool_size 개의 커넥션을 미리 생성한다
	bool Initialize(const DbConfig& config, Factory factory);
	void Finalize();

	bool Execute(const std::string& query, DbRows& out_rows);
	bool ExecuteNonQuery(const std::string& query);

	// worker thread에서 실행, 완료 시 callback 호출
	void ExecuteAsync(const std::string& query, DbCallback callback);

private:
	IDbConnector* AcquireConnection();
	void          ReleaseConnection(IDbConnector* conn);
	void          WorkerLoop();

	struct Task
	{
		std::string query;
		DbCallback  callback;
	};

	DbConfig                                   config_;
	std::vector<std::unique_ptr<IDbConnector>> pool_;
	std::queue<IDbConnector*>                  free_pool_;
	std::mutex                                 pool_mutex_;
	std::condition_variable                    pool_cv_;

	std::queue<Task>        task_queue_;
	std::mutex              task_mutex_;
	std::condition_variable task_cv_;

	std::vector<std::thread> workers_;
	std::atomic<bool>        running_{ false };
};

} // namespace makga::lib::database
