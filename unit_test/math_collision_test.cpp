#include "pch.h"
#include "CppUnitTest.h"

import makga.math.collision;
import makga.math.vector3;
import makga.lib.ecs.component.geometry;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(MathCollision)
	{
	public:
		TEST_METHOD(PointSphereTest)
		{
			try
			{
				makga::lib::Point point;
				point.position = makga::math::Vector3(0.0f, 0.0f, 0.0f);

				makga::lib::Sphere sphere;
				sphere.center = makga::math::Vector3(5.0f, 0.0f, 0.0f);
				sphere.radius = 10.0f;

				bool result = makga::math::CollisionDetector::PointSphere(point, sphere);
				Assert::IsTrue(result, L"Point should be inside sphere");
			}
			catch (...)
			{
				Assert::Fail(L"PointSphereTest exception");
			}
		}

		TEST_METHOD(PointBoxTest)
		{
			try
			{
				makga::lib::Point point;
				point.position = makga::math::Vector3(1.0f, 1.0f, 1.0f);

				makga::lib::Box box;
				box.min = makga::math::Vector3(0.0f, 0.0f, 0.0f);
				box.max = makga::math::Vector3(4.0f, 4.0f, 4.0f);

				bool result = makga::math::CollisionDetector::PointBox(point, box);
				Assert::IsTrue(result, L"Point should be inside box");
			}
			catch (...)
			{
				Assert::Fail(L"PointBoxTest exception");
			}
		}

		TEST_METHOD(SphereSphereTest)
		{
			try
			{
				makga::lib::Sphere a;
				a.center = makga::math::Vector3(0.0f, 0.0f, 0.0f);
				a.radius = 5.0f;

				makga::lib::Sphere b;
				b.center = makga::math::Vector3(8.0f, 0.0f, 0.0f);
				b.radius = 5.0f;

				auto info = makga::math::CollisionDetector::SphereSphere(a, b);
				Assert::IsTrue(info.is_colliding, L"Spheres should collide");
			}
			catch (...)
			{
				Assert::Fail(L"SphereSphereTest exception");
			}
		}

		TEST_METHOD(BoxBoxTest)
		{
			try
			{
				makga::lib::Box a;
				a.min = makga::math::Vector3(0.0f, 0.0f, 0.0f);
				a.max = makga::math::Vector3(2.0f, 2.0f, 2.0f);

				makga::lib::Box b;
				b.min = makga::math::Vector3(1.0f, 0.0f, 0.0f);
				b.max = makga::math::Vector3(2.0f, 2.0f, 2.0f);

				bool result = makga::math::CollisionDetector::BoxBox(a, b);
				Assert::IsTrue(result, L"Boxes should collide");
			}
			catch (...)
			{
				Assert::Fail(L"BoxBoxTest exception");
			}
		}

		TEST_METHOD(RaySphereTest)
		{
			try
			{
				makga::math::Vector3 origin(0.0f, 0.0f, 0.0f);
				makga::math::Vector3 direction(1.0f, 0.0f, 0.0f);
				direction.Normalize();

				makga::lib::Sphere sphere;
				sphere.center = makga::math::Vector3(10.0f, 0.0f, 0.0f);
				sphere.radius = 2.0f;

				float t;
				bool result = makga::math::CollisionDetector::RaySphere(origin, direction, sphere, t);
				Assert::IsTrue(result, L"Ray should hit sphere");
				Assert::IsTrue(t > 0.0f, L"Intersection should be ahead of ray origin");
			}
			catch (...)
			{
				Assert::Fail(L"RaySphereTest exception");
			}
		}

		TEST_METHOD(ReflectTest)
		{
			try
			{
				makga::math::Vector3 velocity(1.0f, -1.0f, 0.0f);
				makga::math::Vector3 normal(0.0f, 1.0f, 0.0f);

				auto reflected = makga::math::CollisionResponse::Reflect(velocity, normal);
				Assert::IsTrue(reflected.GetY() > 0.0f, L"Reflected vector should bounce upward");
			}
			catch (...)
			{
				Assert::Fail(L"ReflectTest exception");
			}
		}
	};
}
