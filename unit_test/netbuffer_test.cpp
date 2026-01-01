#include "pch.h"
#include "CppUnitTest.h"
#include <format>
#include <vector>
#include <string>
#include <array>

import makga.lib.convert;
import makga.network.buffer;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(NetPacket)
	{
	public:
		TEST_METHOD(NetPacketTest)
		{
			try
			{
				std::size_t buffer_size = 1024;
				makga::network::NetPacket buffer;
				buffer.AllocateBuffer(buffer_size);

				Assert::IsTrue(0 == buffer.UsingSize());
				Assert::IsTrue(buffer_size == buffer.AvailableWriteSize());

				int int_value = 123456789;
				buffer.Write(reinterpret_cast<char*>(&int_value), sizeof(int_value));

				Assert::IsTrue(sizeof(int_value) == buffer.UsingSize());
				Assert::IsTrue((buffer_size - sizeof(int_value)) == buffer.AvailableWriteSize());

				std::array<char, 4> read_data = { 0, };
				buffer.Read(read_data.data(), read_data.size());

				Assert::IsTrue(0 == buffer.UsingSize());
				buffer.PullBuffer();
				Assert::IsTrue(buffer_size == buffer.AvailableWriteSize());

				int read_int_value = *reinterpret_cast<int*>(read_data.data());
				Assert::IsTrue(int_value == read_int_value);
			}
			catch (const std::exception& e)
			{
				Assert::Fail(std::format(L"RandomTest exception: {0}", makga::lib::StringToWString(e.what())).c_str());
			}
			catch (...)
			{
				Assert::Fail(L"RandomTest exception");
			}
		}
	};
}