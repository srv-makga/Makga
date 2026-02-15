module;

export module makga.math.collision;

import makga.math.vector3;
import makga.lib.ecs.component.geometry;

export namespace makga::math {
// 충돌 결과 정보
export struct CollisionInfo
{
    bool is_colliding;           // 충돌 여부
    makga::math::Vector3 point;  // 충돌 지점
    makga::math::Vector3 normal; // 충돌 법선
    float penetration;           // 침투 깊이
};

// 충돌 검사 클래스
export class CollisionDetector
{
public:
    // 점과 구체 충돌
    static bool PointSphere(const makga::lib::Point& point, const makga::lib::Sphere& sphere);
    
    // 점과 박스 충돌
    static bool PointBox(const makga::lib::Point& point, const makga::lib::Box& box);
    
    // 구체와 구체 충돌
    static CollisionInfo SphereSphere(const makga::lib::Sphere& a, const makga::lib::Sphere& b);
    
    // 구체와 박스 충돌
    static CollisionInfo SphereBox(const makga::lib::Sphere& sphere, const makga::lib::Box& box);
    
    // 박스와 박스 충돌 (AABB)
    static bool BoxBox(const makga::lib::Box& a, const makga::lib::Box& b);
    
    // 선분과 구체 충돌
    static bool LineSphere(const makga::lib::Line& line, const makga::lib::Sphere& sphere, float& t);
    
    // 선분과 박스 충돌
    static bool LineBox(const makga::lib::Line& line, const makga::lib::Box& box, float& t);
    
    // 광선과 구체 충돌
    static bool RaySphere(const makga::math::Vector3& origin, const makga::math::Vector3& direction, const makga::lib::Sphere& sphere, float& t);
    
    // 광선과 박스 충돌
    static bool RayBox(const makga::math::Vector3& origin, const makga::math::Vector3& direction, const makga::lib::Box& box, float& t);
    
    // 광선과 평면 충돌
    static bool RayPlane(const makga::math::Vector3& origin, const makga::math::Vector3& direction, const makga::math::Vector3& plane_point, const makga::math::Vector3& plane_normal, float& t);
    
    // 캡슐과 캡슐 충돌
    static CollisionInfo CapsuleCapsule(const makga::lib::Capsule& a, const makga::lib::Capsule& b);
    
    // 구체와 캡슐 충돌
    static CollisionInfo SphereCapsule(const makga::lib::Sphere& sphere, const makga::lib::Capsule& capsule);

private:
    // 박스에 가장 가까운 점
    static makga::math::Vector3 ClosestPointOnBox(const makga::math::Vector3& point, const makga::lib::Box& box);
};

// 충돌 응답 계산
export class CollisionResponse
{
public:
    // 탄성 충돌 후 속도 계산
    static void ElasticCollision(makga::math::Vector3& velocity_a, float mass_a, makga::math::Vector3& velocity_b, float mass_b, const makga::math::Vector3& normal);
    
    // 반사 벡터 계산
    static makga::math::Vector3 Reflect(const makga::math::Vector3& velocity, const makga::math::Vector3& normal);
    
    // 침투 해결 (위치 보정)
    static void ResolvePenetration(makga::math::Vector3& position_a, float mass_a, makga::math::Vector3& position_b, float mass_b, const makga::math::Vector3& normal, float penetration);
};

} // namespace makga::math
