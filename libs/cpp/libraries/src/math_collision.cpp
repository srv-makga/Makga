module;

#include <cmath>
#include <algorithm>

module makga.math.collision;

import makga.math.vector3;
import makga.lib.ecs.component.geometry;

namespace makga::math {
// 점과 구체 충돌
bool CollisionDetector::PointSphere(const makga::lib::Point& point, const makga::lib::Sphere& sphere)
{
	float dist_sq = point.position.SquareDistance(sphere.center);
	return dist_sq <= (sphere.radius * sphere.radius);
}

// 점과 박스 충돌
bool CollisionDetector::PointBox(const makga::lib::Point& point, const makga::lib::Box& box)
{
	return (point.position.x_ >= box.min.x_ && point.position.x_ <= box.max.x_ &&
			point.position.y_ >= box.min.y_ && point.position.y_ <= box.max.y_ &&
			point.position.z_ >= box.min.z_ && point.position.z_ <= box.max.z_);
}

// 구체와 구체 충돌
CollisionInfo CollisionDetector::SphereSphere(const makga::lib::Sphere& a, const makga::lib::Sphere& b)
{
	CollisionInfo info{};
	
	makga::math::Vector3 diff = b.center - a.center;
	float dist_sq = diff.x_ * diff.x_ + diff.y_ * diff.y_ + diff.z_ * diff.z_;
	float radius_sum = a.radius + b.radius;
	
	info.is_colliding = (dist_sq <= radius_sum * radius_sum);
	if (true == info.is_colliding)
	{
		float dist = std::sqrt(dist_sq);
		if (0.0f < dist)
		{
			info.normal = diff / dist;
			info.penetration = radius_sum - dist;
			info.point = a.center + info.normal * a.radius;
		}
	}
	
	return info;
}

// 박스에 가장 가까운 점
makga::math::Vector3 CollisionDetector::ClosestPointOnBox(const makga::math::Vector3& point, const makga::lib::Box& box)
{
	makga::math::Vector3 result;
	result.x_ = std::max(box.min.x_, std::min(point.x_, box.max.x_));
	result.y_ = std::max(box.min.y_, std::min(point.y_, box.max.y_));
	result.z_ = std::max(box.min.z_, std::min(point.z_, box.max.z_));
	return result;
}

// 구체와 박스 충돌
CollisionInfo CollisionDetector::SphereBox(const makga::lib::Sphere& sphere, const makga::lib::Box& box)
{
	CollisionInfo info{};
	
	makga::math::Vector3 closest = ClosestPointOnBox(sphere.center, box);
	float dist_sq = sphere.center.SquareDistance(closest);
	
	info.is_colliding = (dist_sq <= sphere.radius * sphere.radius);
	if (true == info.is_colliding)
	{
		float dist = std::sqrt(dist_sq);
		if (0.0f < dist)
		{
			makga::math::Vector3 diff = sphere.center - closest;
			info.normal = diff / dist;
			info.penetration = sphere.radius - dist;
			info.point = closest;
		}
	}
	
	return info;
}

// 박스와 박스 충돌 (AABB)
bool CollisionDetector::BoxBox(const makga::lib::Box& a, const makga::lib::Box& b)
{
	return (a.min.x_ <= b.max.x_ && a.max.x_ >= b.min.x_ &&
			a.min.y_ <= b.max.y_ && a.max.y_ >= b.min.y_ &&
			a.min.z_ <= b.max.z_ && a.max.z_ >= b.min.z_);
}

// 선분과 구체 충돌
bool CollisionDetector::LineSphere(const makga::lib::Line& line, const makga::lib::Sphere& sphere, float& t)
{
	makga::math::Vector3 dir = line.end - line.start;
	makga::math::Vector3 to_sphere = sphere.center - line.start;
	
	float a = dir.Dot(dir);
	float b = 2.0f * to_sphere.Dot(dir);
	float c = to_sphere.Dot(to_sphere) - sphere.radius * sphere.radius;
	
	float discriminant = b * b - 4.0f * a * c;
	if (0.0f > discriminant)
	{
		return false;
	}
	
	t = (-b - std::sqrt(discriminant)) / (2.0f * a);
	return (t >= 0.0f && t <= 1.0f);
}

// 광선과 구체 충돌
bool CollisionDetector::RaySphere(const makga::math::Vector3& origin, const makga::math::Vector3& direction, const makga::lib::Sphere& sphere, float& t)
{
	makga::math::Vector3 to_sphere = sphere.center - origin;
	
	float tca = to_sphere.Dot(direction);
	if (0.0f > tca)
	{
		return false;
	}
	
	float d2 = to_sphere.Dot(to_sphere) - tca * tca;
	float radius2 = sphere.radius * sphere.radius;
	
	if (d2 > radius2)
	{
		return false;
	}
	
	float thc = std::sqrt(radius2 - d2);
	t = tca - thc;
	
	return t >= 0.0f;
}

// 광선과 박스 충돌
bool CollisionDetector::RayBox(const makga::math::Vector3& origin, const makga::math::Vector3& direction, const makga::lib::Box& box, float& t)
{
	float tmin = 0.0f;
	float tmax = std::numeric_limits<float>::max();
	
	for (int i = 0; i < 3; ++i)
	{
		float dir_component = (i == 0) ? direction.x_ : (i == 1) ? direction.y_ : direction.z_;
		float origin_component = (i == 0) ? origin.x_ : (i == 1) ? origin.y_ : origin.z_;
		float box_min = (i == 0) ? box.min.x_ : (i == 1) ? box.min.y_ : box.min.z_;
		float box_max = (i == 0) ? box.max.x_ : (i == 1) ? box.max.y_ : box.max.z_;
		
		if (std::abs(dir_component) < 1e-6f)
		{
			if (origin_component < box_min || origin_component > box_max)
			{
				return false;
			}
		}
		else
		{
			float t1 = (box_min - origin_component) / dir_component;
			float t2 = (box_max - origin_component) / dir_component;
			
			if (t1 > t2) std::swap(t1, t2);
			
			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);
			
			if (tmin > tmax)
			{
				return false;
			}
		}
	}
	
	t = tmin;
	return true;
}

// 광선과 평면 충돌
bool CollisionDetector::RayPlane(const makga::math::Vector3& origin, const makga::math::Vector3& direction, const makga::math::Vector3& plane_point, const makga::math::Vector3& plane_normal, float& t)
{
	float denom = direction.Dot(plane_normal);
	
	if (std::abs(denom) < 1e-6f)
	{
		return false;
	}
	
	makga::math::Vector3 to_plane = plane_point - origin;
	t = to_plane.Dot(plane_normal) / denom;
	
	return t >= 0.0f;
}

// 캡슐과 캡슐 충돌
CollisionInfo CollisionDetector::CapsuleCapsule(const makga::lib::Capsule& a, const makga::lib::Capsule& b)
{
	CollisionInfo info;
	info.is_colliding = false;
	
	float s, t;
	makga::math::Vector3 closest_a = a.start.ClosestDistanceSquaredBetweenLineSegments(a.end, b.start, b.end, s, t);
	makga::math::Vector3 point_a = a.start + (a.end - a.start) * s;
	makga::math::Vector3 point_b = b.start + (b.end - b.start) * t;
	
	makga::math::Vector3 diff = point_b - point_a;
	float dist_sq = diff.Dot(diff);
	float radius_sum = a.radius + b.radius;
	
	info.is_colliding = (dist_sq <= radius_sum * radius_sum);
	if (true == info.is_colliding)
	{
		float dist = std::sqrt(dist_sq);
		if (0.0f < dist)
		{
			info.normal = diff / dist;
			info.penetration = radius_sum - dist;
			info.point = point_a + info.normal * a.radius;
		}
	}
	
	return info;
}

// 구체와 캡슐 충돌
CollisionInfo CollisionDetector::SphereCapsule(const makga::lib::Sphere& sphere, const makga::lib::Capsule& capsule)
{
	CollisionInfo info{};
	
	makga::math::Vector3 closest = sphere.center.ClosestPointOnLineSegment(capsule.start, capsule.end);
	float dist_sq = sphere.center.SquareDistance(closest);
	float radius_sum = sphere.radius + capsule.radius;
	
	info.is_colliding = (dist_sq <= radius_sum * radius_sum);
	if (true == info.is_colliding)
	{
		float dist = std::sqrt(dist_sq);
		if (0.0f < dist)
		{
			makga::math::Vector3 diff = sphere.center - closest;
			info.normal = diff / dist;
			info.penetration = radius_sum - dist;
			info.point = closest + info.normal * capsule.radius;
		}
	}
	
	return info;
}

// 탄성 충돌 후 속도 계산
void CollisionResponse::ElasticCollision(makga::math::Vector3& velocity_a, float mass_a, makga::math::Vector3& velocity_b, float mass_b, const makga::math::Vector3& normal)
{
	float total_mass = mass_a + mass_b;
	
	float va_normal = velocity_a.Dot(normal);
	float vb_normal = velocity_b.Dot(normal);
	
	float new_va_normal = (va_normal * (mass_a - mass_b) + 2.0f * mass_b * vb_normal) / total_mass;
	float new_vb_normal = (vb_normal * (mass_b - mass_a) + 2.0f * mass_a * va_normal) / total_mass;
	
	velocity_a = velocity_a + normal * (new_va_normal - va_normal);
	velocity_b = velocity_b + normal * (new_vb_normal - vb_normal);
}

// 반사 벡터 계산
makga::math::Vector3 CollisionResponse::Reflect(const makga::math::Vector3& velocity, const makga::math::Vector3& normal)
{
	return velocity - normal * (2.0f * velocity.Dot(normal));
}

// 침투 해결 (위치 보정)
void CollisionResponse::ResolvePenetration(makga::math::Vector3& position_a, float mass_a, makga::math::Vector3& position_b, float mass_b, const makga::math::Vector3& normal, float penetration)
{
	float total_mass = mass_a + mass_b;
	float ratio_a = mass_b / total_mass;
	float ratio_b = mass_a / total_mass;
	
	position_a = position_a - normal * (penetration * ratio_a);
	position_b = position_b + normal * (penetration * ratio_b);
}
} // namespace makga::math
