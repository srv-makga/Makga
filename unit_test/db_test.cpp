#include "pch.h"
#include "CppUnitTest.h"
#include <format>
#include <string>
#include <thread>
#include <chrono>

import makga.lib.convert;
import makga.lib.database.redis.connector;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(database)
	{
	public:
		TEST_METHOD(RedisTest)
		{
			try
			{
				makga::lib::database::RedisConnector connector("127.0.0.1", 6379);
				Assert::IsTrue(true == connector.Connect());
				Assert::IsTrue(true == connector.IsConnected());

				connector.Unlink("unittest_key");
				Assert::IsTrue(std::nullopt == connector.Get("unittest_key"));

				connector.Set("unittest_key", "unittest_value");
				Assert::IsTrue("unittest_value" == connector.Get("unittest_key"));

				connector.SetEx("unittest_key", "unittest_value_ex", std::chrono::seconds(3));
				std::this_thread::sleep_for(std::chrono::seconds(4));

				Assert::IsTrue(std::nullopt == connector.Get("unittest_key"));

				connector.Disconnect();
				Assert::IsTrue(false == connector.IsConnected());
			}
			catch (const std::exception& e)
			{
				std::wstring msg;
				makga::lib::Convert(e.what(), msg);
				Assert::Fail(std::format(L"RedisTest exception: {0}", msg).c_str());
			}
			catch (...)
			{
				Assert::Fail(L"RedisTest exception");
			}
		}
	};
}