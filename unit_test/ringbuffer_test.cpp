#include "pch.h"
#include "CppUnitTest.h"
#include <format>
#include <vector>
#include <string>

import makga.lib.convert;
import makga.lib.ring_buffer;
import makga.lib.algorithm.ahocorasick;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(ringbuffer)
	{
	public:
		TEST_METHOD(RingBufferTest)
		{
			size_t buffer_size = 10;
			makga::lib::RingBuffer<int> rb;

			try
			{
				//// 초기 상태
				Assert::IsTrue(!rb.ValidBuffer());

				rb.AllocateBuffer(buffer_size);
				Assert::IsTrue(rb.ValidBuffer());

				size_t real_buffer_size = rb.BufferSize();
				size_t available_size = rb.AvailableWriteSize();

				Assert::IsTrue(real_buffer_size == buffer_size);
				Assert::IsTrue(available_size == rb.AvailableWriteSize());

				//// 삽입
				int a = 10;
				rb.Write(&a, 1);

				size_t used_size = rb.UsingSize();
				available_size = rb.AvailableWriteSize();

				Assert::IsTrue(used_size == 1);
				Assert::IsTrue(real_buffer_size - 1 == available_size);

				std::unique_ptr<int[]> arr(new int[4]);
				size_t read_size = rb.Read(arr.get(), 4);
				Assert::IsTrue(read_size == 0);
			}
			catch (const std::exception& e)
			{
				std::wstring msg;
				makga::lib::Convert(e.what(), msg);
				Assert::Fail(std::format(L"RingBufferTest exception: {0}", msg).c_str());
			}
			catch (...)
			{
				Assert::Fail(L"RingBufferTest exception");
			}
		}
	};
}