#ifndef __linux__

#ifndef _MINI_DUMP_CLASS
#define _MINI_DUMP_CLASS
#define NOMINMAX
#include <Windows.h>
#include <string>
#include <time.h>
#include <DbgHelp.h>

#pragma comment(lib,"DbgHelp.Lib")

class MiniDump
{
public:
	//파일 이름 만들기
	std::string MakeMiniDumpName();

	//오류상황에 따른 미니덤프파일 제작 필터
	static LONG WINAPI CustomExceptionFilter(__in PEXCEPTION_POINTERS pExceptionPointer);

	//오류상황 처리를 위한 필터 등록
	void StartMiniDump() { SetUnhandledExceptionFilter(CustomExceptionFilter); }
};

std::string MiniDump::MakeMiniDumpName()
{
	time_t currentTime;
	struct tm timeInfo;

	std::string dumpFilename;

	char modulePath[MAX_PATH] = { 0, };
	char dateString[MAX_PATH] = { 0, };

	time(&currentTime);
	localtime_s(&timeInfo, &currentTime);

	snprintf(dateString, sizeof(char) * MAX_PATH, "%d_%02d_%02d_%02d_%02d_%02d",
		timeInfo.tm_year + 1900,
		timeInfo.tm_mon + 1,
		timeInfo.tm_mday,
		timeInfo.tm_hour,
		timeInfo.tm_min,
		timeInfo.tm_sec);

	if (::GetModuleFileNameA(0, modulePath, sizeof(modulePath) / sizeof(char)) == 0)
	{
		return std::string();
	}

	dumpFilename = modulePath;
	size_t index = dumpFilename.rfind(".exe");
	dumpFilename.erase(index, 4);

	dumpFilename = dumpFilename + "_" + dateString + ".dmp";

	return dumpFilename;
}

LONG WINAPI MiniDump::CustomExceptionFilter(__in PEXCEPTION_POINTERS pExceptionPointer)
{
	MiniDump miniDump;
	MINIDUMP_EXCEPTION_INFORMATION miniDumpExceptionInformation;
	std::string dumpFilename;

	miniDumpExceptionInformation.ThreadId = ::GetCurrentThreadId();
	miniDumpExceptionInformation.ExceptionPointers = pExceptionPointer;
	miniDumpExceptionInformation.ClientPointers = FALSE;

	dumpFilename = miniDump.MakeMiniDumpName();
	if (dumpFilename.empty() == true)
	{
		::TerminateProcess(::GetCurrentProcess(), 0);
	}

	HANDLE hDumpFile = ::CreateFile(dumpFilename.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	MiniDumpWriteDump(GetCurrentProcess(),
		GetCurrentProcessId(),
		hDumpFile,
		MiniDumpNormal,
		&miniDumpExceptionInformation,
		NULL,
		NULL);

	CloseHandle(hDumpFile);
	::TerminateProcess(::GetCurrentProcess(), 0);

	return 0;
}

#endif
#endif // #ifndef __linux__