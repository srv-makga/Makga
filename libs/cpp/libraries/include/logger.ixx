module;

#include <format>
#include <sstream>
#include <fstream>

export module makga.lib.logger;

import <string>;
import <unordered_map>;
import <mutex>;

import makga.lib.util;
import makga.lib.pattern.singleton;

namespace makga::lib {
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

	template<typename T>
	MakgaLoggerSteam& operator<<(T&& data)
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

	static void Debug(std::string&& log) { level_streams_[LogLevel::DEBUG] << log; }
	static void Info(std::string&& log) { level_streams_[LogLevel::INFO] << log; }
	static void Warn(std::string&& log) { level_streams_[LogLevel::WARN] << log; }
	static void Error(std::string&& log) { level_streams_[LogLevel::ERROR] << log; }
	static void Fatal(std::string&& log) { level_streams_[LogLevel::FATAL] << log; }

	template <typename... Args>
	static void Debug(const std::format_string<Args...> format, Args&&... args)
	{
		level_streams_[LogLevel::DEBUG] << std::vformat(format.get(), _STD make_format_args(args...));
	}

	template <typename... Args>
	static void Info(const std::format_string<Args...> format, Args&&... args)
	{
		level_streams_[LogLevel::INFO] << std::vformat(format.get(), _STD make_format_args(args...));
	}

	template <typename... Args>
	static void Warn(const std::format_string<Args...> format, Args&&... args)
	{
		level_streams_[LogLevel::WARN] << std::vformat(format.get(), _STD make_format_args(args...));
	}

	template <typename... Args>
	static void Error(const std::format_string<Args...> format, Args&&... args)
	{
		level_streams_[LogLevel::ERROR] << std::vformat(format.get(), _STD make_format_args(args...));
	}

	template <typename... Args>
	static void Fatal(const std::format_string<Args...> format, Args&&... args)
	{
		level_streams_[LogLevel::FATAL] << std::vformat(format.get(), _STD make_format_args(args...));
	}

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