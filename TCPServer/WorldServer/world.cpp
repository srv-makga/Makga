#include "pch.h"
#include "../Core/net_service.h"
#include "../Core/MiniDump.h"
#include "server_world.h"
#include "config_world.h"
#include "setting_world.h"
#include "data_manager.h"

int main(int argc, char** argv)
{
	// 마우스 선택으로 프로세스 멈춤 방지
	DWORD prev_mode;
	::GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &prev_mode);
	::SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), prev_mode & ~ENABLE_QUICK_EDIT_MODE);

	// ################################################
	//  기본 정보
	// ################################################
	
	fb::eServerType server_type = fb::eServerType_World;
	
	SETTING.Initialize(server_type);

	MiniDump mini_dump;
	mini_dump.StartMiniDump();

	if (false == CONFIG.Parsing(server_type, argc, argv))
	{
		LOG_FATAL << "config parsing error";
		return false;
	}

	SetConsoleTitle(CONFIG.app_name.c_str());

	std::string log_suffix = std::string(fb::EnumNameeServerType(server_type)) + "_" + std::string(argv[1]);

	core::logger::LoggerGoogle logger;
	logger.Initialize("log", fb::EnumNameeServerType(server_type), log_suffix, ".log");
	logger.set_file_with_console(true);
	logger.set_console_color(true);
	logger.set_prefix(true);

	// ################################################
	// 매니저 초기화
	if (false == DATA_MANAGER.Load())
	{
		LOG_FATAL << "data load fail";
		return 0;
	}

	// ################################################
	//  풀링 정보
	// ################################################

	INIT_POOL(IOContext_t)(1000, 10);
	INIT_POOL(Job)(1000, 10);
	INIT_POOL(NetPacket)(1000, 10);

	// ################################################
	// 서버 정보
	// ################################################

	ServerWorld server(&CONFIG);
	server.Initialize();

	// 서버 실행
	if (false == server.Start())
	{
		LOG_ERROR << "server start fail";
		return 0;
	}

	server.Wait();

	// 서버 종료
	server.Finallize();

	// ################################################
	// 릴리즈
	// ################################################

	_CrtDumpMemoryLeaks();

	return 0;
}