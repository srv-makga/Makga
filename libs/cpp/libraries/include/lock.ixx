module;

export module makga.lib.lock;

import <mutex>;
import <shared_mutex>;

export namespace makga::lib {
using Mutex = std::mutex;
using SharedMutex = std::shared_mutex;
using ReadLock = ::std::shared_lock<SharedMutex>;
using WriteLock = ::std::unique_lock<SharedMutex>;
} // namespace makga::lib