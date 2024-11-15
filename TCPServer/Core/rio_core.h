#pragma once

#include "core_header.h"
#include "lock.h"
#include "rio_session.h"
#include <unordered_set>
#include <memory>

namespace core {
namespace network {
class RioCore : public std::enable_shared_from_this<RioCore>
{
public:
	using Session_t = std::shared_ptr<RioSession>;
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

	void AddSession(Session_t _session);
	void ReleaseSession(Session_t _session);

	RIO_CQ& GetCompletionQueue() { return m_rio_completion_queue; }

private:
	RWMutex m_lock_session;
	std::unordered_set<Session_t> m_sessions;

	bool m_is_init;

	RIO_CQ m_rio_completion_queue;
	RIORESULT m_results[MAX_RIO_RESULT];

public: // static
	inline static RIO_EXTENSION_FUNCTION_TABLE rio = {};
};
} // namespace network
} // namespace core