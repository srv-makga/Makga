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
	TEST_CLASS(unittest)
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
				Assert::Fail(std::format(L"RingBufferTest exception: {0}", makga::lib::StringToWString(e.what())).c_str());
			}
			catch (...)
			{
				Assert::Fail(L"RingBufferTest exception");
			}
		}

		TEST_METHOD(AhocorasickTest)
		{
			try
			{
				makga::lib::TrieNode root;

				std::vector<std::string> patterns = { "he", "she", "his", "hers" };
				for (const auto& p : patterns)
				{
					root.InsertString(p);
				}

				root.ConstructFailureLinks();

				std::string text = "ushers";
				Assert::IsTrue(root.IsMatch(text));

				text = "abc";
				Assert::IsFalse(root.IsMatch(text));

				text = "haha";
				Assert::IsFalse(root.IsMatch(text));

				text = "hers";
				Assert::IsTrue(root.IsMatch(text));
			}
			catch (const std::exception& e)
			{
				Assert::Fail(std::format(L"AhocorasickTest exception: {0}", makga::lib::StringToWString(e.what())).c_str());
			}
			catch (...)
			{
				Assert::Fail(L"AhocorasickTest exception");
			}
		}
	};
}