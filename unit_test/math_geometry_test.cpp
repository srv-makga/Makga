#include "pch.h"
#include "CppUnitTest.h"

import makga.math.geometry;
import makga.math.vector3;
import makga.lib.ecs.component.geometry;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(MathGeometry)
	{
	public:
		TEST_METHOD(TriangleAreaTest)
		{
			try
			{
				makga::math::Vector3 a(0.0f, 0.0f, 0.0f);
				makga::math::Vector3 b(4.0f, 0.0f, 0.0f);
				makga::math::Vector3 c(0.0f, 3.0f, 0.0f);

				float area = makga::math::GeometryMeasure::TriangleArea(a, b, c);
				Assert::IsTrue(area > 0.0f, L"Triangle area should be positive");
				Assert::AreEqual(6.0f, area, 0.01f, L"Triangle area should be 6.0");
			}
			catch (...)
			{
				Assert::Fail(L"TriangleAreaTest exception");
			}
		}

		TEST_METHOD(CircleAreaTest)
		{
			try
			{
				float radius = 5.0f;
				float area = makga::math::GeometryMeasure::CircleArea(radius);
				float expected = 3.14159265f * radius * radius;
				Assert::AreEqual(expected, area, 0.01f, L"Circle area should match πr²");
			}
			catch (...)
			{
				Assert::Fail(L"CircleAreaTest exception");
			}
		}

		TEST_METHOD(SphereVolumeTest)
		{
			try
			{
				float radius = 3.0f;
				float volume = makga::math::GeometryMeasure::SphereVolume(radius);
				Assert::IsTrue(volume > 0.0f, L"Sphere volume should be positive");
			}
			catch (...)
			{
				Assert::Fail(L"SphereVolumeTest exception");
			}
		}

		TEST_METHOD(BoxVolumeTest)
		{
			try
			{
				makga::lib::Box box;
				box.min = makga::math::Vector3(0.0f, 0.0f, 0.0f);
				box.max = makga::math::Vector3(2.0f, 3.0f, 4.0f);

				float volume = makga::math::GeometryMeasure::BoxVolume(box);
				Assert::AreEqual(24.0f, volume, 0.01f, L"Box volume should be 24.0");
			}
			catch (...)
			{
				Assert::Fail(L"BoxVolumeTest exception");
			}
		}

		TEST_METHOD(AngleBetweenVectorsTest)
		{
			try
			{
				makga::math::Vector3 a(1.0f, 0.0f, 0.0f);
				makga::math::Vector3 b(0.0f, 1.0f, 0.0f);

				float angle = makga::math::GeometryCalculation::AngleBetweenVectorsDegrees(a, b);
				Assert::AreEqual(90.0f, angle, 0.1f, L"Angle should be 90 degrees");
			}
			catch (...)
			{
				Assert::Fail(L"AngleBetweenVectorsTest exception");
			}
		}

		TEST_METHOD(ProjectOnPlaneTest)
		{
			try
			{
				makga::math::Vector3 vector(1.0f, 1.0f, 0.0f);
				makga::math::Vector3 planeNormal(0.0f, 1.0f, 0.0f);

				auto projected = makga::math::GeometryCalculation::ProjectOnPlane(vector, planeNormal);
				Assert::AreEqual(0.0f, projected.GetY(), 0.01f, L"Y component should be 0 after projection");
			}
			catch (...)
			{
				Assert::Fail(L"ProjectOnPlaneTest exception");
			}
		}

		TEST_METHOD(PointToPlaneDistanceTest)
		{
			try
			{
				makga::math::Vector3 point(0.0f, 5.0f, 0.0f);
				makga::math::Vector3 planePoint(0.0f, 0.0f, 0.0f);
				makga::math::Vector3 planeNormal(0.0f, 1.0f, 0.0f);

				float distance = makga::math::GeometryCalculation::PointToPlaneDistance(point, planePoint, planeNormal);
				Assert::AreEqual(5.0f, distance, 0.01f, L"Distance should be 5.0");
			}
			catch (...)
			{
				Assert::Fail(L"PointToPlaneDistanceTest exception");
			}
		}

		TEST_METHOD(PlaneFromPointsTest)
		{
			try
			{
				makga::math::Vector3 a(0.0f, 0.0f, 0.0f);
				makga::math::Vector3 b(1.0f, 0.0f, 0.0f);
				makga::math::Vector3 c(0.0f, 1.0f, 0.0f);

				auto normal = makga::math::PlaneCalculation::PlaneFromPoints(a, b, c);
				float length = normal.Length();
				Assert::IsTrue(length > 0.0f, L"Normal should have positive length");
			}
			catch (...)
			{
				Assert::Fail(L"PlaneFromPointsTest exception");
			}
		}
	};
}
