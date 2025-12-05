#include "pch.h"
#include "CppUnitTest.h"

import makga.lib.ring_buffer;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(unittest)
	{
	public:
		
		TEST_METHOD(RingBufferTest)
		{
			try
			{
				//makga::lib::ring_buffer<int> rb(3);

				//// 초기 상태
				//Assert::IsTrue(rb.empty());
				//Assert::IsTrue(rb.size() == 0);

				//// 삽입
				//rb.push_back(10);
				//rb.push_back(20);
				//rb.push_back(30);

				//Assert::IsFalse(rb.empty());
				//Assert::IsTrue(rb.size() == 3);

				//// 앞/뒤 검사
				//Assert::AreEqual(10, rb.front());
				//Assert::AreEqual(30, rb.back());

				//// 제거 (FIFO)
				//rb.pop_front();
				//Assert::IsTrue(rb.size() == 2);
				//Assert::AreEqual(20, rb.front());

				//// 남은 요소 제거
				//rb.pop_front();
				//rb.pop_front();
				//Assert::IsTrue(rb.empty());
				//Assert::IsTrue(rb.size() == 0);

				//// clear 검사 (있다면)
				//rb.push_back(1);
				//rb.push_back(2);
				//rb.clear();
				//Assert::IsTrue(rb.empty());
			}
			catch (const std::exception& e)
			{
				Assert::Fail(L"Unhandled std::exception in RingBufferTest");
			}
			catch (...)
			{
				Assert::Fail(L"Unhandled exception in RingBufferTest");
			}
		}
	};
}