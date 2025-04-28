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

/* glog �÷���
* logtostderr		            (bool  , default = false     ) �α� ������ �ƴ� stderr�� �޽����� �����.
* alsologtostderr	            (bool  , default = false     ) �α� �޽����� �α� ���ϰ� stderr�� �Բ� �����.
* colorlogtostderr              (bool  , default = false     ) stderr �޽����� ������ �����Ѵ�.
* stderrthreshold               (int   , default = GLOG_ERROR) ������ ������ �� ���� �������� �α� �޽������� �α� ���� �ܿ� stderr �� ���縦 �Ѵ�.
* log_prefix                    (bool  , default = true      ) �� �ٿ� �α� ���λ縦 �߰����� ���θ� �߰��Ѵ�.
* logbuflevel                   (int   , default = 0         ) �ش� ���� ������ �޽����� ���۸��ǰ�, ���� ������ ��� flush�ȴ�.
* logbufsecs                    (int   , default = 30        ) �αװ� ���۸��� �� �ִ� �ִ� �ð��� �����Ѵ�.
* minloglevel                   (int   , default = 0         ) ������ ������ �� ���� �������� �޽����� ���Ͽ� �����.
* log_dir                       (string, default = ""        ) ���� ��õǸ� �α� ���ϵ��� �⺻ ���丮 ��ſ� �� ���丮�� ��������.
* log_link                      (string, default = ""        ) �α� ���Ͽ� ���� �߰� ��ũ�� ���� ������ ��θ� �����Ѵ�.
* max_log_size                  (int   , default = 1800      ) �ִ� �α� ���� ũ��(MB)�� �����Ѵ�.
* stop_logging_if_full_disk		(bool  , default = false     ) ��ũ�� ���� á�� �� ��ũ�� ������� ������ ���θ� �����Ѵ�.
*/

#define LOG_FILE_INFO_FORMAT		__func__ << ") "

#define LOG_DEBUG					LOG(DEBUG) << LOG_FILE_INFO_FORMAT
#define LOG_INFO					LOG(INFO) << LOG_FILE_INFO_FORMAT
#define LOG_WARN					LOG(WARNING) << LOG_FILE_INFO_FORMAT
#define LOG_ERROR					LOG(ERROR) << LOG_FILE_INFO_FORMAT
#define LOG_FATAL					LOG(FATAL) << LOG_FILE_INFO_FORMAT

// cond�� true�� �� log ����
#define LOG_DEBUG_IF(cond)			LOG_IF(DEBUG, cond) << LOG_FILE_INFO_FORMAT
#define LOG_INFO_IF(cond)			LOG_IF(INFO, cond) << LOG_FILE_INFO_FORMAT
#define LOG_WARN_IF(cond)			LOG_IF(WARNING, cond) << LOG_FILE_INFO_FORMAT
#define LOG_ERROR_IF(cond)			LOG_IF(ERROR, cond) << LOG_FILE_INFO_FORMAT
#define LOG_FATAL_IF(cond)			LOG_IF(FATAL, cond) << LOG_FILE_INFO_FORMAT

// ����� ��忡���� �α�
#define DLOG_DEBUG					DLOG(DEBUG) << LOG_FILE_INFO_FORMAT
#define DLOG_INFO					DLOG(INFO) << LOG_FILE_INFO_FORMAT
#define DLOG_WARN					DLOG(WARNING) << LOG_FILE_INFO_FORMAT
#define DLOG_ERROR					DLOG(ERROR) << LOG_FILE_INFO_FORMAT
#define DLOG_FATAL					DLOG(FATAL) << LOG_FILE_INFO_FORMAT

// cond�� true�� �� log ����
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
	* @brief �α� ���ϵ��� ����� ���丮 ����
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

	// @brief �α� ������ �ƴ� �ַܼ� �޽����� �����.
	void set_console(bool _flag)
	{
		fLB::FLAGS_logtostderr = _flag;
	}

	// @brief �α� �޽����� �α� ���ϰ� �ֿܼ� �Բ� �����.
	void set_file_with_console(bool _flag)
	{
		fLB::FLAGS_alsologtostderr = _flag;
	}

	// @brief �ܼ� �޽����� ������ �����Ѵ�.
	void set_console_color(bool _flag)
	{
		fLB::FLAGS_colorlogtostderr = _flag;
	}

	// @brief ������ ������ �� ���� �������� �α� �޽������� �α� ���� �ܿ� stderr �� ���縦 �Ѵ�.
	void set_console_higher_level(int _level)
	{
		fLI::FLAGS_stderrthreshold = _level;
	}

	// @brief �� �ٿ� �α� ���λ縦 �߰����� ���θ� �߰��Ѵ�.
	void set_prefix(bool _flag)
	{
		fLB::FLAGS_log_prefix = _flag;
	}

	// @brief �ش� ���� ������ �޽����� ���۸��ǰ�, ���� ������ ��� flush�ȴ�.
	void set_buf_level(int _level)
	{
		fLI::FLAGS_logbuflevel = _level;
	}

	// @brief �αװ� ���۸��� �� �ִ� �ִ� �ð��� �����Ѵ�.
	void set_buf_time(int _time)
	{
		fLI::FLAGS_logbufsecs = _time;
	}

	// @brief ������ ������ �� ���� �������� �޽����� ���Ͽ� �����.
	void set_min_level(int _level)
	{
		fLI::FLAGS_minloglevel = _level;
	}

	// @brief �ִ� �α� ���� ũ��(MB)�� �����Ѵ�.
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