#include "pch.h"
#include "CppUnitTest.h"
#include <format>
#include <vector>
#include <string>

import makga.lib.convert;
import makga.lib.binary_writer;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(binarywriter)
	{
	public:
		TEST_METHOD(BinaryWriterTest)
		{
			try
			{
				makga::lib::BinaryWriter writer(1024);

				Assert::IsTrue(writer.Write<int32_t>(123456789));
				Assert::IsTrue(writer.Write<float>(3.14159f));
				Assert::IsTrue(writer.Write<std::string>("Hello, BinaryWriter!"));
				Assert::IsTrue(writer.Write<int32_t>(123456789));
				Assert::IsTrue(writer.GetSize() > 0);

				const char* data = writer.GetData();
				Assert::IsNotNull(data);

				int32_t v = 0;
				Assert::IsTrue(writer.Read<int32_t>(v));
				Assert::IsTrue(123456789 == v);

				float f = 0.0f;
				Assert::IsTrue(writer.Read<float>(f));
				Assert::IsTrue(3.14159f == f);

				std::string str;
				Assert::IsTrue(writer.Read<std::string>(str));
				Assert::IsTrue(0 == str.compare("Hello, BinaryWriter!"));

				v = 0;
				Assert::IsTrue(writer.Read<int32_t>(v));
				Assert::IsTrue(123456789 == v);
			}
			catch (const std::exception& e)
			{
				std::wstring msg;
				makga::lib::Convert(e.what(), msg);
				Assert::Fail(std::format(L"BinaryWriterTest exception: {0}", msg).c_str());
			}
			catch (...)
			{
				Assert::Fail(L"BinaryWriterTest exception");
			}
		}
	};
}