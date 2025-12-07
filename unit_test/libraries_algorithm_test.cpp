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
	TEST_CLASS(algorithm)
	{
	public:
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