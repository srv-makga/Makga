#pragma once

#include "core_header.h"
#include "net_core.h"
#include "lock.h"
#include <unordered_set>
#include <memory>

namespace core {
namespace network {
class RioSession;
class RioCore : public std::enable_shared_from_this<RioCore>, public NetCore
{
public:
	RioCore();
	RioCore(const RioCore&) = delete;
	RioCore(RioCore&&) = delete;
	RioCore& operator=(const RioCore&) = delete;
	RioCore& operator=(RioCore&&) = delete;
	~RioCore() = default;

	bool Initialize();
	void Finalize();

	bool Dispatch();
	void DeferredSend();

	void AddSession(std::shared_ptr<RioSession> _session);
	void ReleaseSession(std::shared_ptr<RioSession> _session);

	RIO_CQ& GetCompletionQueue() { return m_rio_completion_queue; }

public: // NetCore
	CoreType GetCoreType() const override { return CoreType::IOCP; }

private:
	RWMutex m_lock_session;
	std::unordered_set<std::shared_ptr<RioSession>> m_sessions;

	bool m_is_init;

	RIO_CQ m_rio_completion_queue;
	RIORESULT m_results[MAX_RIO_RESULT];

public: // static
	inline static RIO_EXTENSION_FUNCTION_TABLE rio = {};
};
} // namespace network
} // namespace core