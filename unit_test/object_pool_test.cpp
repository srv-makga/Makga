#include "pch.h"
#include "CppUnitTest.h"
#include <atomic>

import makga.lib.pattern.objectpool;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	struct TestObj
	{
		TestObj() { ++s_count; }
		~TestObj() { --s_count; }
		int value = 0;
		static std::atomic<int> s_count;
	};

	std::atomic<int> TestObj::s_count{ 0 };

	TEST_CLASS(ObjectPool)
	{
	public:
		TEST_METHOD(ObjectPoolBasic)
		{
			try
			{
				makga::lib::ObjectPool<TestObj> pool;
				pool.CreateObject(2);
				Assert::IsTrue(TestObj::s_count.load() == 2);

				{
					auto o1 = pool.AcquireObject();
					auto o2 = pool.AcquireObject();
					// using objects
					o1->value = 10;
					o2->value = 20;
				}

				// After release deleters should have returned objects to pool (no new allocations)
				{
					auto o3 = pool.AcquireObject();
					auto o4 = pool.AcquireObject();
					Assert::IsTrue(TestObj::s_count.load() == 2);
				}

				pool.Finalize();
				Assert::IsTrue(TestObj::s_count.load() == 0);
			}
			catch (const std::exception& e)
			{
				Assert::Fail(L"ObjectPoolBasic exception");
			}
			catch (...)
			{
				Assert::Fail(L"ObjectPoolBasic exception");
			}
		}
	};
}
