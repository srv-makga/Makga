#include "pch.h"
#include "CppUnitTest.h"

import makga.lib.pattern.singleton;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	class MySingleton
	{
	public:
		int value = 0;
	};

	TEST_CLASS(Singleton)
	{
	public:
		TEST_METHOD(SingletonBasic)
		{
			try
			{
				MySingleton& a = makga::lib::Singleton<MySingleton>::Instance();
				a.value = 5;
				MySingleton& b = makga::lib::Singleton<MySingleton>::Get();
				Assert::IsTrue(&a == &b);
				Assert::IsTrue(b.value == 5);
			}
			catch (...) { Assert::Fail(L"SingletonBasic exception"); }
		}
	};
}
