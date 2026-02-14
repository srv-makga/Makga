#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
    TEST_CLASS(UdpPeerTests)
    {
    public:
        TEST_METHOD(SimpleTrue)
        {
            Assert::IsTrue(true);
        }
    };
}