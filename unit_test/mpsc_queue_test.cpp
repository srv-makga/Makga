#include "pch.h"
#include "CppUnitTest.h"

import makga.lib.mpsc_queue;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(MPSCQueue)
	{
	public:
		TEST_METHOD(MPSCQueueBasic)
		{
			try
			{
				makga::lib::MPSCQueue<int> q;
				Assert::IsTrue(q.IsEmpty());

				q.Enqueue(10);
				q.Enqueue(20);
				q.Enqueue(30);

				int v = 0;
				Assert::IsTrue(q.Dequeue(v) && v == 10);
				Assert::IsTrue(q.Dequeue(v) && v == 20);
				Assert::IsTrue(q.Dequeue(v) && v == 30);
				Assert::IsTrue(!q.Dequeue(v));
			}
			catch (...) { Assert::Fail(L"MPSCQueueBasic exception"); }
		}
	};
}
