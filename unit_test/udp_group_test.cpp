#include "pch.h"
#include "CppUnitTest.h"

import makga.network.udp.group;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(UdpGroupTests)
	{
	public:
		TEST_METHOD(AddRemoveGetMembers)
		{
			try
			{
				makga::network::UdpGroup g;
				g.AddMember("room1", "127.0.0.1", 5000);
				g.AddMember("room1", "127.0.0.1", 5001);
				auto m = g.GetMembers("room1");
				Assert::IsTrue(m.size() == 2);

				g.RemoveMember("room1", "127.0.0.1", 5000);
				auto m2 = g.GetMembers("room1");
				Assert::IsTrue(m2.size() == 1);
			}
			catch (...) { Assert::Fail(L"UdpGroupTests exception"); }
		}
	};
}
