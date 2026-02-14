#include "pch.h"
#include "CppUnitTest.h"
#include <format>

import makga.math.vector3;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(Vector3)
	{
	public:
		TEST_METHOD(Vector3Basic)
		{
			try
			{
				makga::math::Vector3 a(1.0f, 2.0f, 3.0f);
				makga::math::Vector3 b(4.0f, 5.0f, 6.0f);

				// Distance
				float dist = a.Distance(b);
				Assert::IsTrue(dist > 0.0f);

				// Dot
				float dot = a.Dot(b);
				Assert::IsTrue(dot == (1.0f*4.0f + 2.0f*5.0f + 3.0f*6.0f));

				// Cross
				auto c = a.Cross(b);
				Assert::IsTrue(c.GetX() == (2.0f*6.0f - 3.0f*5.0f));

				// Operators
				auto sum = a + b;
				Assert::IsTrue(sum.GetX() == 5.0f && sum.GetY() == 7.0f && sum.GetZ() == 9.0f);

				auto mul = a * 2.0f;
				Assert::IsTrue(mul.GetX() == 2.0f && mul.GetY() == 4.0f && mul.GetZ() == 6.0f);
			}
			catch (const std::exception&)
			{
				Assert::Fail(L"Vector3Basic exception");
			}
			catch (...)
			{
				Assert::Fail(L"Vector3Basic exception");
			}
		}
	};
}
