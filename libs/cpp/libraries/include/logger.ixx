module;

#include <mutex>
#include <sstream>
#include <fstream>

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
			// @todo 시간, 스레드, 함수명등
			std::lock_guard lock(mutex);
			stream << data;
		}
		return *this;
	}
};

export class MakgaLogger : public Logger, public Singleton<MakgaLogger>
{
public:
	MakgaLogger() = default;
	virtual ~MakgaLogger() = default;

	inline static MakgaLoggerSteam& Debug() { return level_streams_[LogLevel::DEBUG]; }
	inline static MakgaLoggerSteam& Info() { return level_streams_[LogLevel::INFO]; }
	inline static MakgaLoggerSteam& Warn() { return level_streams_[LogLevel::WARN]; }
	inline static MakgaLoggerSteam& Error() { return level_streams_[LogLevel::ERROR]; }
	inline static MakgaLoggerSteam& Fatal() { return level_streams_[LogLevel::FATAL]; }

protected:
	inline static std::unordered_map<LogLevel, MakgaLoggerSteam> level_streams_;

	// @todo 파일 이름, 크기등 설정 추가 필요
};
} // namespace makga::lib