#include "pch.h"
#include "CppUnitTest.h"

import makga.lib.doublequeue;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(DoubleQueue)
	{
	public:
		TEST_METHOD(DoubleQueueBasic)
		{
			try
			{
				makga::lib::DoubleQueue<int*> dq;

				int a = 1, b = 2, c = 3;
				dq.Push(&a);
				dq.Push(&b);
				dq.Push(&c);

				int* out = nullptr;
				out = dq.Pop();
				Assert::IsTrue(out == &a);
				out = dq.Pop();
				Assert::IsTrue(out == &b);
				out = dq.Pop();
				Assert::IsTrue(out == &c);
			}
			catch (...) { Assert::Fail(L"DoubleQueueBasic exception"); }
		}
	};
}
