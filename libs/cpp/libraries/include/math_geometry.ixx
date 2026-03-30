module;

#include <cmath>

export module makga.math.geometry;

import makga.math.vector3;
import makga.lib.ecs.component.geometry;

export namespace makga::math {
// 넓이 및 부피 계산
export class GeometryMeasure
{
public:
	// 삼각형 넓이 (3개의 점)
	static float TriangleArea(const makga::math::Vector3& a, const makga::math::Vector3& b, const makga::math::Vector3& c);
	
	// 사각형 넓이 (4개의 점, 평면 가정)
	static float QuadArea(const makga::math::Vector3& a, const makga::math::Vector3& b, const makga::math::Vector3& c, const makga::math::Vector3& d);
	
	// 원 넓이
	static float CircleArea(float radius);
	
	// 타원 넓이
	static float EllipseArea(float major_axis, float minor_axis);
	
	// 구체 표면적
	static float SphereArea(float radius);
	
	// 구체 부피
	static float SphereVolume(float radius);
	
	// 박스 표면적
	static float BoxArea(const makga::lib::Box& box);
	
	// 박스 부피
	static float BoxVolume(const makga::lib::Box& box);
	
	// 캡슐 부피
	static float CapsuleVolume(float radius, float height);
	
	// 캡슐 표면적
	static float CapsuleArea(float radius, float height);
	
	// 원기둥 부피
	static float CylinderVolume(float radius, float height);
	
	// 원기둥 표면적
	static float CylinderArea(float radius, float height);
	
	// 원뿔 부피
	static float ConeVolume(float radius, float height);
	
	// 원뿔 표면적
	static float ConeArea(float radius, float height);
};

// 거리 및 각도 계산
export class GeometryCalculation
{
public:
	// 점과 선 사이의 거리
	static float PointToLineDistance(const makga::math::Vector3& point, const makga::lib::Line& line);
	
	// 점과 평면 사이의 거리
	static float PointToPlaneDistance(const makga::math::Vector3& point, const makga::math::Vector3& plane_point, const makga::math::Vector3& plane_normal);
	
	// 두 선분 사이의 최단 거리
	static float LineToLineDistance(const makga::lib::Line& line1, const makga::lib::Line& line2);
	
	// 두 벡터 사이의 각도 (라디안)
	static float AngleBetweenVectors(const makga::math::Vector3& a, const makga::math::Vector3& b);
	
	// 두 벡터 사이의 각도 (도)
	static float AngleBetweenVectorsDegrees(const makga::math::Vector3& a, const makga::math::Vector3& b);
	
	// 벡터의 평면 투영
	static makga::math::Vector3 ProjectOnPlane(const makga::math::Vector3& vector, const makga::math::Vector3& plane_normal);
	
	// 벡터의 벡터 투영
	static makga::math::Vector3 ProjectOnVector(const makga::math::Vector3& vector, const makga::math::Vector3& target);
	
	// 점이 삼각형 내부에 있는지 (중심좌표계)
	static bool IsPointInTriangle(const makga::math::Vector3& point, const makga::math::Vector3& a, const makga::math::Vector3& b, const makga::math::Vector3& c);
	
	// 무게중심 좌표 계산
	static makga::math::Vector3 BarycentricCoordinates(const makga::math::Vector3& point, const makga::math::Vector3& a, const makga::math::Vector3& b, const makga::math::Vector3& c);
};

// 교점 계산
export class Intersection
{
public:
	// 두 선의 교점 (2D, XZ 평면)
	static bool LineLineIntersection2D(const makga::math::Vector3& a1, const makga::math::Vector3& a2, const makga::math::Vector3& b1, const makga::math::Vector3& b2, makga::math::Vector3& intersection);
	
	// 선과 평면의 교점
	static bool LinePlaneIntersection(const makga::math::Vector3& line_start, const makga::math::Vector3& line_end, const makga::math::Vector3& plane_point, const makga::math::Vector3& plane_normal, makga::math::Vector3& intersection);
	
	// 세 평면의 교점
	static bool ThreePlaneIntersection(const makga::math::Vector3& point1, const makga::math::Vector3& normal1, const makga::math::Vector3& point2, const makga::math::Vector3& normal2, const makga::math::Vector3& point3, const makga::math::Vector3& normal3, makga::math::Vector3& intersection);
};

// 평면 계산
export class PlaneCalculation
{
public:
	// 세 점에서 평면 생성 (법선 반환)
	static makga::math::Vector3 PlaneFromPoints(const makga::math::Vector3& a, const makga::math::Vector3& b, const makga::math::Vector3& c);
	
	// 평면과 점의 관계 (양수: 앞, 0: 위, 음수: 뒤)
	static float DistanceToPlane(const makga::math::Vector3& point, const makga::math::Vector3& plane_point, const makga::math::Vector3& plane_normal);
	
	// 점이 평면의 어느 쪽에 있는지
	static int WhichSideOfPlane(const makga::math::Vector3& point, const makga::math::Vector3& plane_point, const makga::math::Vector3& plane_normal);
};
} // namespace makga::math
