#include "pch.h"
#include "CppUnitTest.h"
#include <format>
#include <vector>
#include <string>

import makga.lib.convert;
import makga.lib.datetime;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(datetime)
	{
	public:
		TEST_METHOD(DatetimeTest)
		{
			try
			{
				std::time_t now = std::time(nullptr);
				makga::lib::DateTime dt(now);

				Assert::IsTrue(dt == now);
				Assert::IsFalse(dt < now);

				dt.AddSec(10).CalculateTime();

				Assert::IsFalse(dt == now);
				Assert::IsTrue(dt > now);

				dt.SetDateTime("1970-01-01 00:00:00").CalculateTime();
				Assert::IsTrue(dt == std::time_t());
				Assert::IsTrue(0 == dt.String().compare("1970-01-01 00:00:00"));
			}
			catch (const std::exception& e)
			{
				Assert::Fail(std::format(L"DatetimeTest exception: {0}", makga::lib::StringToWString(e.what())).c_str());
			}
			catch (...)
			{
				Assert::Fail(L"DatetimeTest exception");
			}
		}
	};
}