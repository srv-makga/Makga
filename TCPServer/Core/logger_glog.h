#pragma once

#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES
#ifdef ERROR
#undef ERROR
#endif

#include "logger.h"
#include "glog/logging.h"
#include <filesystem>
#include "singleton.hpp"

/* glog 플래그
* logtostderr		            (bool  , default = false     ) 로그 파일이 아닌 stderr로 메시지를 남긴다.
* alsologtostderr	            (bool  , default = false     ) 로그 메시지를 로그 파일과 stderr에 함께 남긴다.
* colorlogtostderr              (bool  , default = false     ) stderr 메시지에 색상을 설정한다.
* stderrthreshold               (int   , default = GLOG_ERROR) 지정한 레벨과 그 위의 레벨들의 로그 메시지들을 로그 파일 외에 stderr 로 복사를 한다.
* log_prefix                    (bool  , default = true      ) 각 줄에 로그 접두사를 추가할지 여부를 추가한다.
* logbuflevel                   (int   , default = 0         ) 해당 레벨 이하의 메시지는 버퍼링되고, 상위 레벨은 즉시 flush된다.
* logbufsecs                    (int   , default = 30        ) 로그가 버퍼링될 수 있는 최대 시간을 설정한다.
* minloglevel                   (int   , default = 0         ) 지정한 레벨과 그 위의 레벨들의 메시지를 파일에 남긴다.
* log_dir                       (string, default = ""        ) 값이 명시되면 로그 파일들은 기본 디렉토리 대신에 이 디렉토리에 남겨진다.
* log_link                      (string, default = ""        ) 로그 파일에 대한 추가 링크를 넣을 디렉토의 경로를 설정한다.
* max_log_size                  (int   , default = 1800      ) 최대 로그 파일 크기(MB)를 설정한다.
* stop_logging_if_full_disk		(bool  , default = false     ) 디스크가 가득 찼을 때 디스크에 기록하지 않을지 여부를 설정한다.
*/

#define LOG_FILE_INFO_FORMAT		__func__ << ") "

#define LOG_DEBUG					LOG(DEBUG) << LOG_FILE_INFO_FORMAT
#define LOG_INFO					LOG(INFO) << LOG_FILE_INFO_FORMAT
#define LOG_WARN					LOG(WARNING) << LOG_FILE_INFO_FORMAT
#define LOG_ERROR					LOG(ERROR) << LOG_FILE_INFO_FORMAT
#define LOG_FATAL					LOG(FATAL) << LOG_FILE_INFO_FORMAT

// cond가 true일 때 log 남김
#define LOG_DEBUG_IF(cond)			LOG_IF(DEBUG, cond) << LOG_FILE_INFO_FORMAT
#define LOG_INFO_IF(cond)			LOG_IF(INFO, cond) << LOG_FILE_INFO_FORMAT
#define LOG_WARN_IF(cond)			LOG_IF(WARNING, cond) << LOG_FILE_INFO_FORMAT
#define LOG_ERROR_IF(cond)			LOG_IF(ERROR, cond) << LOG_FILE_INFO_FORMAT
#define LOG_FATAL_IF(cond)			LOG_IF(FATAL, cond) << LOG_FILE_INFO_FORMAT

// 디버그 모드에서만 로그
#define DLOG_DEBUG					DLOG(DEBUG) << LOG_FILE_INFO_FORMAT
#define DLOG_INFO					DLOG(INFO) << LOG_FILE_INFO_FORMAT
#define DLOG_WARN					DLOG(WARNING) << LOG_FILE_INFO_FORMAT
#define DLOG_ERROR					DLOG(ERROR) << LOG_FILE_INFO_FORMAT
#define DLOG_FATAL					DLOG(FATAL) << LOG_FILE_INFO_FORMAT

// cond가 true일 때 log 남김
#define DLOG_DEBUG_IF(cond)			DLOG_IF(DEBUG, cond) << LOG_FILE_INFO_FORMAT
#define DLOG_INFO_IF(cond)			DLOG_IF(INFO, cond) << LOG_FILE_INFO_FORMAT
#define DLOG_WARN_IF(cond)			DLOG_IF(WARNING, cond) << LOG_FILE_INFO_FORMAT
#define DLOG_ERROR_IF(cond)			DLOG_IF(ERROR, cond) << LOG_FILE_INFO_FORMAT
#define DLOG_FATAL_IF(cond)			DLOG_IF(FATAL, cond) << LOG_FILE_INFO_FORMAT

#define LOG_USER(user)				" user_uid:" << user->UserUid()
#define LOG_RESULT(result)			" " << fb::EnumNameeResult(result)

namespace core {
namespace logger {
class LoggerGoogle : public Logger, public core::pattern::Singleton<LoggerGoogle>
{
public:
	LoggerGoogle()
		: Logger()
	{
	}

	~LoggerGoogle()
	{
		google::ShutdownGoogleLogging();
	}

	bool Initialize(const std::string& _directory, const std::string& _name, const std::string& _suffix, const std::string& _ext)
	{
		SetDirectory(_directory + "\\" + _name);

		std::string full_name = fLS::FLAGS_log_dir + _suffix + std::string("_");

		google::InitGoogleLogging("");
		google::SetLogDestination(google::GLOG_DEBUG, full_name.c_str());
		google::SetLogDestination(google::GLOG_INFO, full_name.c_str());
		google::SetLogDestination(google::GLOG_WARNING, full_name.c_str());
		google::SetLogDestination(google::GLOG_ERROR, full_name.c_str());
		google::SetLogDestination(google::GLOG_FATAL, full_name.c_str());

		google::SetLogFilenameExtension(_ext.c_str());
		return true;
	}

	void Finalize()
	{
		google::ShutdownGoogleLogging();
	}

	/*
	* @brief 로그 파일들을 기록할 디렉토리 설정
	*/
	void SetDirectory(const std::string& _directory)
	{
		std::filesystem::create_directories(_directory);

		fLS::FLAGS_log_dir = _directory;
		fLS::FLAGS_log_dir += "\\";
	}

	template<typename... Args>
	void Info(Args&&... _args)
	{
		(LOG(INFO) << ... << _args);
	}

	template<typename... Args>
	void Warn(Args&&... _args)
	{
		(LOG(WARNING) << ... << _args);
	}

	template<typename... Args>
	void Error(Args&&... _args)
	{
		(LOG(ERROR) << ... << _args);
	}

	template<typename... Args>
	void Fatal(Args&&... _args)
	{
		(LOG(FATAL) << ... << _args);
	}

	template<typename... Args>
	void DebugInfo(Args&&... _args)
	{
		(DLOG(INFO) << ... << _args);
	}

	template<typename... Args>
	void DebugWarn(Args&&... _args)
	{
		(DLOG(WARNING) << ... << _args);
	}

	template<typename... Args>
	void DebugError(Args&&... _args)
	{
		(DLOG(ERROR) << ... << _args);
	}

	template<typename... Args>
	void DebugFatal(Args&&... _args)
	{
		(DLOG(FATAL) << ... << _args);
	}

	// @brief 로그 파일이 아닌 콘솔로 메시지를 남긴다.
	void set_console(bool _flag)
	{
		fLB::FLAGS_logtostderr = _flag;
	}

	// @brief 로그 메시지를 로그 파일과 콘솔에 함께 남긴다.
	void set_file_with_console(bool _flag)
	{
		fLB::FLAGS_alsologtostderr = _flag;
	}

	// @brief 콘솔 메시지에 색상을 설정한다.
	void set_console_color(bool _flag)
	{
		fLB::FLAGS_colorlogtostderr = _flag;
	}

	// @brief 지정한 레벨과 그 위의 레벨들의 로그 메시지들을 로그 파일 외에 stderr 로 복사를 한다.
	void set_console_higher_level(int _level)
	{
		fLI::FLAGS_stderrthreshold = _level;
	}

	// @brief 각 줄에 로그 접두사를 추가할지 여부를 추가한다.
	void set_prefix(bool _flag)
	{
		fLB::FLAGS_log_prefix = _flag;
	}

	// @brief 해당 레벨 이하의 메시지는 버퍼링되고, 상위 레벨은 즉시 flush된다.
	void set_buf_level(int _level)
	{
		fLI::FLAGS_logbuflevel = _level;
	}

	// @brief 로그가 버퍼링될 수 있는 최대 시간을 설정한다.
	void set_buf_time(int _time)
	{
		fLI::FLAGS_logbufsecs = _time;
	}

	// @brief 지정한 레벨과 그 위의 레벨들의 메시지를 파일에 남긴다.
	void set_min_level(int _level)
	{
		fLI::FLAGS_minloglevel = _level;
	}

	// @brief 최대 로그 파일 크기(MB)를 설정한다.
	void set_max_size(int _size)
	{
		fLI::FLAGS_max_log_size = _size;
	}

private:
	std::string m_prefix;
};

#define Glogger LoggerGoogle::Instance()
} // namespace logger
} // namespace core