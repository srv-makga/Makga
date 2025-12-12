module;

#include <mutex>
#include <sstream>

export module makga.lib.logger;

import <string>;
import <unordered_map>;
import makga.lib.util;
import makga.lib.pattern.singleton;

export namespace makga::lib {
export enum LoggerType
{
	Custom,
	GoogleLog,
};

export enum LogLevel
{
	DEBUG,
	INFO,
	WARN,
	ERROR,
	FATAL
};

// @brief 인터페이스 느낌
export class Logger
{
public:
	Logger() = default;
	virtual ~Logger() = default;
};

struct MakgaLoggerSteam
{
	std::mutex mutex;
	std::ostringstream stream;

	template<typename T>
	MakgaLoggerSteam& operator<<(T& data)
	{
		{
			std::lock_guard lock(mutex);
			stream << data;
		}
		return *this;
	}
};

export class MakgaLogger : public Logger, public Singleton<MakgaLogger>
{
public:
	MakgaLogger();
	virtual ~MakgaLogger() = default;

	inline MakgaLoggerSteam& Debug() { return level_streams_[LogLevel::DEBUG]; }
	inline MakgaLoggerSteam& Info() { return level_streams_[LogLevel::INFO]; }
	inline MakgaLoggerSteam& Warn() { return level_streams_[LogLevel::WARN]; }
	inline MakgaLoggerSteam& Error() { return level_streams_[LogLevel::ERROR]; }
	inline MakgaLoggerSteam& Fatal() { return level_streams_[LogLevel::FATAL]; }

protected:
	std::unordered_map<LogLevel, MakgaLoggerSteam> level_streams_;
};
} // namespace makga::lib