#include "pch.h"
#include "../Core/net_service.h"
#include "../Core/MiniDump.h"
#include "../Core/logger_glog.h"

#include "server_game.h"
#include "app_config.h"
#include "setting_game.h"
#include "pool.h"

#include "world.h"
#include "data_manager.h"

// @todo 클래스화
void InitLogger()
{
	std::string log_suffix = std::format("_{0}_{1}", fb::EnumNameeServerType(eServerType_Game), /*/@todo 서버 ID 가져오기*/ 1);

	core::logger::LoggerGoogle logger;
	logger.Initialize("log", fb::EnumNameeServerType(eServerType_Game), log_suffix, ".log");
	logger.set_file_with_console(true);
	logger.set_console_color(true);
	logger.set_prefix(true);
}

// @todo 클래스화
void InitDump()
{
	MiniDump mini_dump;
	mini_dump.StartMiniDump();
}

int main(int argc, char** argv)
{
	// 메모리 누수 확인
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

	// 마우스 선택으로 프로세스 멈춤 방지
	DWORD prev_mode;
	::GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &prev_mode);
	::SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), prev_mode & ~ENABLE_QUICK_EDIT_MODE);

	InitLogger();
	InitDump();

	eServerType server_type = eServerType_Game;
	ServerId_t serverid = std::atoi(argv[1]);

	std::shared_ptr<AppConfig> config = std::make_shared<AppConfig>();
	if (nullptr == config)
	{
		LOG_FATAL << "config create fail";
		return false;
	}

	if (false == config->Parsing(server_type, serverid))
	{
		LOG_FATAL << "config parsing error";
		return false;
	}

	SetConsoleTitle(config->app_name.c_str());

	// 설정 읽어오기
	SETTING.Initialize(server_type);

	// ################################################
	if (false == GetWorld().Initialize())
		LOG_FATAL << "world initialize fail";
	{
		return 0;
	}

	// ################################################
	// 서버 실행
	// ################################################
	// @todo 싱글톤이라 파라미터가 있으면 생성할 수 없다. 내부에 인스턴스를 static으로 들고있게 하자
	std::shared_ptr<GameServer> server = std::make_shared<GameServer>(core::ServiceType::IOCP_SERVER, config);
	server->Initialize();

	// 서버 실행
	if (false == server->Start())
	{
		LOG_FATAL << "server start fail";
		return 0;
	}

	// ################################################
	// 릴리즈
	// ################################################

	server->Finalize();

	_CrtDumpMemoryLeaks();

	return 0;
}