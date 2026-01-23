#include "pch.h"
#include "CppUnitTest.h"
#include <format>
#include <vector>
#include <string>

import makga.lib.convert;
import makga.lib.random;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(random)
	{
	public:
		TEST_METHOD(RandomTest)
		{
			try
			{
				makga::lib::RandomEngine engine;

				for (int i = 0; i < 1000000; ++i)
				{
					int result = engine.Rand<int>(1, 10);
					Assert::IsTrue((1 <= result && result <= 10));
				}

				for (int i = 0; i < 1000000; ++i)
				{
					float result = engine.Rand<float>(0.5f, 9.9f);
					Assert::IsTrue((0.5f <= result && result <= 9.9f));
				}
			}
			catch (const std::exception& e)
			{
				std::wstring msg;
				makga::lib::Convert(e.what(), msg);
				Assert::Fail(std::format(L"RandomTest exception: {0}", msg).c_str());
			}
			catch (...)
			{
				Assert::Fail(L"RandomTest exception");
			}
		}
	};
}