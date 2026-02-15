#include "pch.h"
#include "CppUnitTest.h"

import makga.math.motion;
import makga.math.vector3;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(MathMotion)
	{
	public:
		TEST_METHOD(LinearMotionTest)
		{
			try
			{
				makga::math::Vector3 initialPos(0.0f, 0.0f, 0.0f);
				makga::math::Vector3 velocity(1.0f, 0.0f, 0.0f);
				float time = 5.0f;

				auto position = makga::math::Motion::LinearMotion(initialPos, velocity, time);
				Assert::AreEqual(5.0f, position.GetX(), 0.01f, L"Position should be 5.0 after 5 seconds");
			}
			catch (...)
			{
				Assert::Fail(L"LinearMotionTest exception");
			}
		}

		TEST_METHOD(AcceleratedMotionTest)
		{
			try
			{
				makga::math::Vector3 initialPos(0.0f, 0.0f, 0.0f);
				makga::math::Vector3 velocity(0.0f, 0.0f, 0.0f);
				makga::math::Vector3 acceleration(2.0f, 0.0f, 0.0f);
				float time = 3.0f;

				auto position = makga::math::Motion::AcceleratedMotion(initialPos, velocity, acceleration, time);
				float expected = 0.5f * 2.0f * 3.0f * 3.0f; // 0.5 * a * t^2
				Assert::AreEqual(expected, position.GetX(), 0.01f, L"Accelerated motion calculation");
			}
			catch (...)
			{
				Assert::Fail(L"AcceleratedMotionTest exception");
			}
		}

		TEST_METHOD(ProjectileMotionTest)
		{
			try
			{
				makga::math::Vector3 initialPos(0.0f, 0.0f, 0.0f);
				makga::math::Vector3 velocity(10.0f, 10.0f, 0.0f);
				float gravity = -9.8f; // Gravity should be negative
				float time = 1.0f;

				auto position = makga::math::Motion::ProjectileMotion(initialPos, velocity, gravity, time);
				Assert::AreEqual(10.0f, position.GetX(), 0.01f, L"X position should be 10.0");
				Assert::IsTrue(position.GetY() < 10.0f, L"Y position should be less than 10.0 due to gravity");
			}
			catch (...)
			{
				Assert::Fail(L"ProjectileMotionTest exception");
			}
		}

		TEST_METHOD(LerpTest)
		{
			try
			{
				makga::math::Vector3 start(0.0f, 0.0f, 0.0f);
				makga::math::Vector3 end(10.0f, 0.0f, 0.0f);

				auto midpoint = makga::math::Motion::Lerp(start, end, 0.5f);
				Assert::AreEqual(5.0f, midpoint.GetX(), 0.01f, L"Lerp at 0.5 should be midpoint");

				auto atStart = makga::math::Motion::Lerp(start, end, 0.0f);
				Assert::AreEqual(0.0f, atStart.GetX(), 0.01f, L"Lerp at 0.0 should be start");

				auto atEnd = makga::math::Motion::Lerp(start, end, 1.0f);
				Assert::AreEqual(10.0f, atEnd.GetX(), 0.01f, L"Lerp at 1.0 should be end");
			}
			catch (...)
			{
				Assert::Fail(L"LerpTest exception");
			}
		}

		TEST_METHOD(SmoothStepTest)
		{
			try
			{
				float t0 = makga::math::Motion::SmoothStep(0.0f);
				float t05 = makga::math::Motion::SmoothStep(0.5f);
				float t1 = makga::math::Motion::SmoothStep(1.0f);

				Assert::AreEqual(0.0f, t0, 0.01f, L"SmoothStep at 0.0 should be 0.0");
				Assert::AreEqual(0.5f, t05, 0.01f, L"SmoothStep at 0.5 should be 0.5");
				Assert::AreEqual(1.0f, t1, 0.01f, L"SmoothStep at 1.0 should be 1.0");
			}
			catch (...)
			{
				Assert::Fail(L"SmoothStepTest exception");
			}
		}

		TEST_METHOD(QuaternionBasicTest)
		{
			try
			{
				makga::math::Rotation::Quaternion q(1.0f, 0.0f, 0.0f, 0.0f);
				auto normalized = q.Normalize();
				Assert::AreEqual(1.0f, normalized.w, 0.01f, L"Normalized quaternion w should be 1.0");
			}
			catch (...)
			{
				Assert::Fail(L"QuaternionBasicTest exception");
			}
		}

		TEST_METHOD(DegToRadTest)
		{
			try
			{
				float deg = 180.0f;
				float rad = makga::math::Rotation::DegToRad(deg);
				Assert::AreEqual(3.14159265f, rad, 0.01f, L"180 degrees should be π radians");

				float backToDeg = makga::math::Rotation::RadToDeg(rad);
				Assert::AreEqual(180.0f, backToDeg, 0.01f, L"Conversion back to degrees");
			}
			catch (...)
			{
				Assert::Fail(L"DegToRadTest exception");
			}
		}

		TEST_METHOD(AverageVelocityTest)
		{
			try
			{
				makga::math::Vector3 displacement(10.0f, 0.0f, 0.0f);
				float time = 2.0f;

				auto velocity = makga::math::Velocity::AverageVelocity(displacement, time);
				Assert::AreEqual(5.0f, velocity.GetX(), 0.01f, L"Average velocity should be 5.0");
			}
			catch (...)
			{
				Assert::Fail(L"AverageVelocityTest exception");
			}
		}
	};
}
