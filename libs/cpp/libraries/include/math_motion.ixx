module;

#include <cmath>

export module makga.math.motion;

import makga.math.vector3;

export namespace makga::math {

// 이동 계산 클래스
export class Motion
{
public:
    // 등속 운동: position = initial_position + velocity * time
    static makga::math::Vector3 LinearMotion(const makga::math::Vector3& initial_position,
                                             const makga::math::Vector3& velocity,
                                             float time);
    
    // 등가속도 운동: position = initial_position + velocity * time + 0.5 * acceleration * time^2
    static makga::math::Vector3 AcceleratedMotion(const makga::math::Vector3& initial_position,
                                                   const makga::math::Vector3& velocity,
                                                   const makga::math::Vector3& acceleration,
                                                   float time);
    
    // 포물선 운동 (중력 적용)
    static makga::math::Vector3 ProjectileMotion(const makga::math::Vector3& initial_position,
                                                  const makga::math::Vector3& initial_velocity,
                                                  float gravity,
                                                  float time);
    
    // 원운동 (2D 평면)
    static makga::math::Vector3 CircularMotion(const makga::math::Vector3& center,
                                                float radius,
                                                float angular_velocity,
                                                float time,
                                                float initial_angle = 0.0f);
    
    // 감쇠 운동 (마찰력 적용)
    static makga::math::Vector3 DampedMotion(const makga::math::Vector3& initial_position,
                                              const makga::math::Vector3& velocity,
                                              float damping_coefficient,
                                              float time);
    
    // 선형 보간 (Lerp)
    static makga::math::Vector3 Lerp(const makga::math::Vector3& start,
                                      const makga::math::Vector3& end,
                                      float t);
    
    // 구면 선형 보간 (Slerp) - 방향 벡터용
    static makga::math::Vector3 Slerp(const makga::math::Vector3& start,
                                       const makga::math::Vector3& end,
                                       float t);
    
    // 스무스 보간 (SmoothStep)
    static float SmoothStep(float t);
    
    // 베지어 곡선 (3차)
    static makga::math::Vector3 CubicBezier(const makga::math::Vector3& p0,
                                             const makga::math::Vector3& p1,
                                             const makga::math::Vector3& p2,
                                             const makga::math::Vector3& p3,
                                             float t);
};

// 회전 계산 클래스
export class Rotation
{
public:
    // 쿼터니언 (회전 표현)
    struct Quaternion
    {
        float w, x, y, z;
        
        Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
        Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
        
        // 정규화
        Quaternion Normalize() const;
        
        // 켤레
        Quaternion Conjugate() const;
        
        // 곱셈
        Quaternion operator*(const Quaternion& other) const;
        
        // 벡터 회전
        makga::math::Vector3 RotateVector(const makga::math::Vector3& v) const;
    };
    
    // 오일러 각에서 쿼터니언 생성 (라디안)
    static Quaternion FromEuler(float pitch, float yaw, float roll);
    
    // 쿼터니언에서 오일러 각 추출 (라디안)
    static void ToEuler(const Quaternion& q, float& pitch, float& yaw, float& roll);
    
    // 축-각 표현에서 쿼터니언 생성
    static Quaternion FromAxisAngle(const makga::math::Vector3& axis, float angle);
    
    // X축 회전 행렬 생성 (3x3)
    static void RotationMatrixX(float angle, float matrix[9]);
    
    // Y축 회전 행렬 생성 (3x3)
    static void RotationMatrixY(float angle, float matrix[9]);
    
    // Z축 회전 행렬 생성 (3x3)
    static void RotationMatrixZ(float angle, float matrix[9]);
    
    // 행렬로 벡터 회전
    static makga::math::Vector3 RotateByMatrix(const makga::math::Vector3& v, const float matrix[9]);
    
    // 쿼터니언 선형 보간 (Slerp)
    static Quaternion Slerp(const Quaternion& start, const Quaternion& end, float t);
    
    // 각도를 라디안으로
    static float DegToRad(float degrees);
    
    // 라디안을 각도로
    static float RadToDeg(float radians);
};

// 속도 및 가속도 계산
export class Velocity
{
public:
    // 평균 속도 계산
    static makga::math::Vector3 AverageVelocity(const makga::math::Vector3& displacement, float time);
    
    // 순간 속도 (가속도 적용)
    static makga::math::Vector3 InstantaneousVelocity(const makga::math::Vector3& initial_velocity,
                                                       const makga::math::Vector3& acceleration,
                                                       float time);
    
    // 평균 가속도 계산
    static makga::math::Vector3 AverageAcceleration(const makga::math::Vector3& velocity_change, float time);
    
    // 각속도에서 선속도 계산
    static makga::math::Vector3 AngularToLinear(const makga::math::Vector3& angular_velocity,
                                                 const makga::math::Vector3& radius_vector);
};

} // namespace makga::math
