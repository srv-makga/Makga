#include "pch.h"
#include "../Core/MiniDump.h"
#include "server_dbagent.h"
#include "config_dbagent.h"
#include "setting_dbagent.h"
#include "data_manager.h"

int main(int argc, char** argv)
{
	fb::eServerType server_type = fb::eServerType_DBAgent;

	// ################################################
	//  �⺻ ����
	// ################################################

	// ���콺 �������� ���μ��� ���� ����
	DWORD prev_mode;
	::GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &prev_mode);
	::SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), prev_mode & ~ENABLE_QUICK_EDIT_MODE);

	SETTING.Initialize(server_type);

	std::string log_suffix = "_" + std::string(fb::EnumNameeServerType(server_type)) + "_" + std::string(argv[1]);

	core::logger::LoggerGoogle logger;
	logger.Initialize("log", fb::EnumNameeServerType(server_type), log_suffix, ".log");
	logger.set_file_with_console(true);
	logger.set_console_color(true);
	logger.set_prefix(true);

	MiniDump mini_dump;
	mini_dump.StartMiniDump();

	if (false == CONFIG.Parsing(server_type, argc, argv))
	{
		LOG_FATAL << "config parsing error";
		return false;
	}

	SetConsoleTitle(CONFIG.app_name);

	// ################################################
	// �Ŵ��� �ʱ�ȭ
	if (false == DATA_MANAGER.Load())
	{
		LOG_FATAL << "data load fail";
		return 0;
	}

	// ################################################
	//  Ǯ�� ����
	// ################################################

	INIT_POOL(IOContext_t)(1000, 10);
	INIT_POOL(Job)(1000, 10);
	INIT_POOL(NetPacket)(1000, 10);

	// ################################################
	// ���� ����
	// ################################################

	ServerDBAgent server(&CONFIG);
	server.Initialize();

	// ���� ����
	if (false == server.Start())
	{
		LOG_ERROR << "server start fail";
		return 0;
	}

	server.Wait();

	// ���� ����
	server.Finallize();

	// ################################################
	// ������
	// ################################################

	_CrtDumpMemoryLeaks();

	return 0;
}