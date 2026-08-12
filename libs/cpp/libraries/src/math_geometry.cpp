module;

#include <iostream>
#include <cmath>

module makga.math.geometry;

import makga.math.vector3;
import makga.lib.ecs.component.geometry;

namespace makga::math {

constexpr float PI = 3.14159265358979323846f;

// GeometryMeasure 구현
float GeometryMeasure::TriangleArea(const makga::math::Vector3& a, const makga::math::Vector3& b, const makga::math::Vector3& c)
{
	makga::math::Vector3 ab = b - a;
	makga::math::Vector3 ac = c - a;
	makga::math::Vector3 cross = ab.Cross(ac);
	
	float area_squared = cross.x_ * cross.x_ + cross.y_ * cross.y_ + cross.z_ * cross.z_;
	return std::sqrt(area_squared) * 0.5f;
}

float GeometryMeasure::QuadArea(const makga::math::Vector3& a, const makga::math::Vector3& b, const makga::math::Vector3& c, const makga::math::Vector3& d)
{
	// 사각형을 두 삼각형으로 분할
	return TriangleArea(a, b, c) + TriangleArea(a, c, d);
}

float GeometryMeasure::CircleArea(float radius)
{
	return PI * radius * radius;
}

float GeometryMeasure::EllipseArea(float major_axis, float minor_axis)
{
	return PI * major_axis * minor_axis;
}

float GeometryMeasure::SphereArea(float radius)
{
	return 4.0f * PI * radius * radius;
}

float GeometryMeasure::SphereVolume(float radius)
{
	return (4.0f / 3.0f) * PI * radius * radius * radius;
}

float GeometryMeasure::BoxArea(const makga::lib::Box& box)
{
	float width = box.max.x_ - box.min.x_;
	float height = box.max.y_ - box.min.y_;
	float depth = box.max.z_ - box.min.z_;
	
	return 2.0f * (width * height + width * depth + height * depth);
}

float GeometryMeasure::BoxVolume(const makga::lib::Box& box)
{
	float width = box.max.x_ - box.min.x_;
	float height = box.max.y_ - box.min.y_;
	float depth = box.max.z_ - box.min.z_;
	
	return width * height * depth;
}

float GeometryMeasure::CapsuleVolume(float radius, float height)
{
	// 캡슐 = 원기둥 + 두 개의 반구
	float cylinder_volume = PI * radius * radius * height;
	float sphere_volume = (4.0f / 3.0f) * PI * radius * radius * radius;
	return cylinder_volume + sphere_volume;
}

float GeometryMeasure::CapsuleArea(float radius, float height)
{
	// 캡슐 = 원기둥 측면 + 구 표면
	float cylinder_area = 2.0f * PI * radius * height;
	float sphere_area = 4.0f * PI * radius * radius;
	return cylinder_area + sphere_area;
}

float GeometryMeasure::CylinderVolume(float radius, float height)
{
	return PI * radius * radius * height;
}

float GeometryMeasure::CylinderArea(float radius, float height)
{
	// 측면 + 위아래 원
	return 2.0f * PI * radius * height + 2.0f * PI * radius * radius;
}

float GeometryMeasure::ConeVolume(float radius, float height)
{
	return (1.0f / 3.0f) * PI * radius * radius * height;
}

float GeometryMeasure::ConeArea(float radius, float height)
{
	float slant_height = std::sqrt(radius * radius + height * height);
	// 측면 + 밑면
	return PI * radius * slant_height + PI * radius * radius;
}

// GeometryCalculation 구현
float GeometryCalculation::PointToLineDistance(const makga::math::Vector3& point, const makga::lib::Line& line)
{
	makga::math::Vector3 line_vec = line.end - line.start;
	makga::math::Vector3 point_vec = point - line.start;
	
	makga::math::Vector3 cross = point_vec.Cross(line_vec);
	float cross_length = std::sqrt(cross.x_ * cross.x_ + cross.y_ * cross.y_ + cross.z_ * cross.z_);
	float line_length = std::sqrt(line_vec.x_ * line_vec.x_ + line_vec.y_ * line_vec.y_ + line_vec.z_ * line_vec.z_);
	
	if (0.0f < line_length)
	{
		return cross_length / line_length;
	}
	
	return point.Distance(line.start);
}

float GeometryCalculation::PointToPlaneDistance(const makga::math::Vector3& point, const makga::math::Vector3& plane_point, const makga::math::Vector3& plane_normal)
{
	makga::math::Vector3 point_vec = point - plane_point;
	return std::abs(point_vec.Dot(plane_normal));
}

float GeometryCalculation::LineToLineDistance(const makga::lib::Line& line1, const makga::lib::Line& line2)
{
	float s, t;
	line1.start.ClosestDistanceSquaredBetweenLineSegments(line1.end, line2.start, line2.end, s, t);
	
	makga::math::Vector3 point1 = line1.start + (line1.end - line1.start) * s;
	makga::math::Vector3 point2 = line2.start + (line2.end - line2.start) * t;
	
	return point1.Distance(point2);
}

float GeometryCalculation::AngleBetweenVectors(const makga::math::Vector3& a, const makga::math::Vector3& b)
{
	float dot = a.Dot(b);
	float len_a = std::sqrt(a.x_ * a.x_ + a.y_ * a.y_ + a.z_ * a.z_);
	float len_b = std::sqrt(b.x_ * b.x_ + b.y_ * b.y_ + b.z_ * b.z_);
	
	if (len_a > 0.0f && len_b > 0.0f)
	{
		float cos_angle = dot / (len_a * len_b);
		cos_angle = std::max(-1.0f, std::min(1.0f, cos_angle));
		return std::acos(cos_angle);
	}
	
	return 0.0f;
}

float GeometryCalculation::AngleBetweenVectorsDegrees(const makga::math::Vector3& a,
													   const makga::math::Vector3& b)
{
	return AngleBetweenVectors(a, b) * 180.0f / PI;
}

makga::math::Vector3 GeometryCalculation::ProjectOnPlane(const makga::math::Vector3& vector,
														  const makga::math::Vector3& plane_normal)
{
	float dot = vector.Dot(plane_normal);
	return vector - plane_normal * dot;
}

makga::math::Vector3 GeometryCalculation::ProjectOnVector(const makga::math::Vector3& vector,
														   const makga::math::Vector3& target)
{
	float dot = vector.Dot(target);
	float target_len_sq = target.x_ * target.x_ + target.y_ * target.y_ + target.z_ * target.z_;
	
	if (target_len_sq > 0.0f)
		return target * (dot / target_len_sq);
	
	return makga::math::Vector3(0.0f, 0.0f, 0.0f);
}

bool GeometryCalculation::IsPointInTriangle(const makga::math::Vector3& point,
											 const makga::math::Vector3& a,
											 const makga::math::Vector3& b,
											 const makga::math::Vector3& c)
{
	makga::math::Vector3 bary = BarycentricCoordinates(point, a, b, c);
	return (bary.x_ >= 0.0f && bary.y_ >= 0.0f && bary.z_ >= 0.0f &&
			bary.x_ + bary.y_ + bary.z_ <= 1.0f + 1e-6f);
}

makga::math::Vector3 GeometryCalculation::BarycentricCoordinates(const makga::math::Vector3& point,
																  const makga::math::Vector3& a,
																  const makga::math::Vector3& b,
																  const makga::math::Vector3& c)
{
	makga::math::Vector3 v0 = b - a;
	makga::math::Vector3 v1 = c - a;
	makga::math::Vector3 v2 = point - a;
	
	float d00 = v0.Dot(v0);
	float d01 = v0.Dot(v1);
	float d11 = v1.Dot(v1);
	float d20 = v2.Dot(v0);
	float d21 = v2.Dot(v1);
	
	float denom = d00 * d11 - d01 * d01;
	
	if (std::abs(denom) < 1e-6f)
		return makga::math::Vector3(1.0f, 0.0f, 0.0f);
	
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;
	
	return makga::math::Vector3(u, v, w);
}

// Intersection 구현
bool Intersection::LineLineIntersection2D(const makga::math::Vector3& a1,
										  const makga::math::Vector3& a2,
										  const makga::math::Vector3& b1,
										  const makga::math::Vector3& b2,
										  makga::math::Vector3& intersection)
{
	float x1 = a1.x_, z1 = a1.z_;
	float x2 = a2.x_, z2 = a2.z_;
	float x3 = b1.x_, z3 = b1.z_;
	float x4 = b2.x_, z4 = b2.z_;
	
	float denom = (x1 - x2) * (z3 - z4) - (z1 - z2) * (x3 - x4);
	
	if (std::abs(denom) < 1e-6f)
		return false;
	
	float t = ((x1 - x3) * (z3 - z4) - (z1 - z3) * (x3 - x4)) / denom;
	
	intersection.x_ = x1 + t * (x2 - x1);
	intersection.y_ = 0.0f;
	intersection.z_ = z1 + t * (z2 - z1);
	
	return true;
}

bool Intersection::LinePlaneIntersection(const makga::math::Vector3& line_start,
										  const makga::math::Vector3& line_end,
										  const makga::math::Vector3& plane_point,
										  const makga::math::Vector3& plane_normal,
										  makga::math::Vector3& intersection)
{
	makga::math::Vector3 line_dir = line_end - line_start;
	float denom = line_dir.Dot(plane_normal);
	
	if (std::abs(denom) < 1e-6f)
		return false;
	
	makga::math::Vector3 to_plane = plane_point - line_start;
	float t = to_plane.Dot(plane_normal) / denom;
	
	if (t < 0.0f || t > 1.0f)
		return false;
	
	intersection = line_start + line_dir * t;
	return true;
}

bool Intersection::ThreePlaneIntersection(const makga::math::Vector3& point1, const makga::math::Vector3& normal1,
										   const makga::math::Vector3& point2, const makga::math::Vector3& normal2,
										   const makga::math::Vector3& point3, const makga::math::Vector3& normal3,
										   makga::math::Vector3& intersection)
{
	// 크레이머 공식 사용
	makga::math::Vector3 cross23 = normal2.Cross(normal3);
	float denom = normal1.Dot(cross23);
	
	if (std::abs(denom) < 1e-6f)
		return false;
	
	float d1 = point1.Dot(normal1);
	float d2 = point2.Dot(normal2);
	float d3 = point3.Dot(normal3);
	
	intersection = (cross23 * d1 + normal3.Cross(normal1) * d2 + normal1.Cross(normal2) * d3) / denom;
	return true;
}

// PlaneCalculation 구현
makga::math::Vector3 PlaneCalculation::PlaneFromPoints(const makga::math::Vector3& a,
														const makga::math::Vector3& b,
														const makga::math::Vector3& c)
{
	makga::math::Vector3 ab = b - a;
	makga::math::Vector3 ac = c - a;
	makga::math::Vector3 normal = ab.Cross(ac);
	
	float length = std::sqrt(normal.x_ * normal.x_ + normal.y_ * normal.y_ + normal.z_ * normal.z_);
	if (length > 0.0f)
		return normal / length;
	
	return makga::math::Vector3(0.0f, 1.0f, 0.0f);
}

float PlaneCalculation::DistanceToPlane(const makga::math::Vector3& point,
										 const makga::math::Vector3& plane_point,
										 const makga::math::Vector3& plane_normal)
{
	makga::math::Vector3 point_vec = point - plane_point;
	return point_vec.Dot(plane_normal);
}

int PlaneCalculation::WhichSideOfPlane(const makga::math::Vector3& point,
										const makga::math::Vector3& plane_point,
										const makga::math::Vector3& plane_normal)
{
	float dist = DistanceToPlane(point, plane_point, plane_normal);
	
	if (dist > 1e-6f)
		return 1;  // 앞
	else if (dist < -1e-6f)
		return -1; // 뒤
	else
		return 0;  // 위
}

} // namespace makga::math
