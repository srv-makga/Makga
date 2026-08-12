module;

#include <iostream>
#include <cmath>

module makga.math.motion;

import makga.math.vector3;

namespace makga::math {
// Motion 구현
makga::math::Vector3 Motion::LinearMotion(const makga::math::Vector3& initial_position, const makga::math::Vector3& velocity, float time)
{
    return initial_position + velocity * time;
}

makga::math::Vector3 Motion::AcceleratedMotion(const makga::math::Vector3& initial_position, const makga::math::Vector3& velocity, const makga::math::Vector3& acceleration, float time)
{
    return initial_position + velocity * time + acceleration * (0.5f * time * time);
}

makga::math::Vector3 Motion::ProjectileMotion(const makga::math::Vector3& initial_position, const makga::math::Vector3& initial_velocity, float gravity, float time)
{
    makga::math::Vector3 gravity_vec(0.0f, gravity, 0.0f);
    return AcceleratedMotion(initial_position, initial_velocity, gravity_vec, time);
}

makga::math::Vector3 Motion::CircularMotion(const makga::math::Vector3& center, float radius, float angular_velocity, float time, float initial_angle)
{
    float angle = initial_angle + angular_velocity * time;
    float x = center.x_ + radius * std::cos(angle);
    float z = center.z_ + radius * std::sin(angle);
    return makga::math::Vector3(x, center.y_, z);
}

makga::math::Vector3 Motion::DampedMotion(const makga::math::Vector3& initial_position, const makga::math::Vector3& velocity, float damping_coefficient, float time)
{
    float damping_factor = std::exp(-damping_coefficient * time);
    return initial_position + velocity * (damping_factor / damping_coefficient) * (1.0f - damping_factor);
}

makga::math::Vector3 Motion::Lerp(const makga::math::Vector3& start, const makga::math::Vector3& end, float t)
{
    t = std::max(0.0f, std::min(1.0f, t));
    return start + (end - start) * t;
}

makga::math::Vector3 Motion::Slerp(const makga::math::Vector3& start, const makga::math::Vector3& end, float t)
{
    float dot = start.Dot(end);
    dot = std::max(-1.0f, std::min(1.0f, dot));
    
    float theta = std::acos(dot) * t;
    
    makga::math::Vector3 relative = end - start * dot;
    float length_sq = relative.x_ * relative.x_ + relative.y_ * relative.y_ + relative.z_ * relative.z_;
    
    if (length_sq > 0.0f)
	{
        float length = std::sqrt(length_sq);
        relative = relative / length;
    }
    
    return start * std::cos(theta) + relative * std::sin(theta);
}

float Motion::SmoothStep(float t)
{
    t = std::max(0.0f, std::min(1.0f, t));
    return t * t * (3.0f - 2.0f * t);
}

makga::math::Vector3 Motion::CubicBezier(const makga::math::Vector3& p0, const makga::math::Vector3& p1, const makga::math::Vector3& p2, const makga::math::Vector3& p3, float t)
{
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    
    return p0 * uuu + p1 * (3.0f * uu * t) + p2 * (3.0f * u * tt) + p3 * ttt;
}

// Rotation::Quaternion 구현
Rotation::Quaternion Rotation::Quaternion::Normalize() const
{
    float length = std::sqrt(w*w + x*x + y*y + z*z);
    if (0.0f < length)
	{
        float inv_length = 1.0f / length;
        return Quaternion(w * inv_length, x * inv_length, y * inv_length, z * inv_length);
    }

    return *this;
}

Rotation::Quaternion Rotation::Quaternion::Conjugate() const
{
    return Quaternion(w, -x, -y, -z);
}

Rotation::Quaternion Rotation::Quaternion::operator*(const Quaternion& other) const
{
    return Quaternion(
        w * other.w - x * other.x - y * other.y - z * other.z,
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w
    );
}

makga::math::Vector3 Rotation::Quaternion::RotateVector(const makga::math::Vector3& v) const
{
    Quaternion v_quat(0, v.x_, v.y_, v.z_);
    Quaternion result = (*this) * v_quat * this->Conjugate();
    return makga::math::Vector3(result.x, result.y, result.z);
}

// Rotation 구현
Rotation::Quaternion Rotation::FromEuler(float pitch, float yaw, float roll)
{
    float cy = std::cos(yaw * 0.5f);
    float sy = std::sin(yaw * 0.5f);
    float cp = std::cos(pitch * 0.5f);
    float sp = std::sin(pitch * 0.5f);
    float cr = std::cos(roll * 0.5f);
    float sr = std::sin(roll * 0.5f);
    
    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;
    
    return q;
}

void Rotation::ToEuler(const Quaternion& q, float& pitch, float& yaw, float& roll)
{
    // Roll (x-axis rotation)
    float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    roll = std::atan2(sinr_cosp, cosr_cosp);
    
    // Pitch (y-axis rotation)
    float sinp = 2.0f * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1.0f)
        pitch = std::copysign(3.14159265f / 2.0f, sinp);
    else
        pitch = std::asin(sinp);
    
    // Yaw (z-axis rotation)
    float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    yaw = std::atan2(siny_cosp, cosy_cosp);
}

Rotation::Quaternion Rotation::FromAxisAngle(const makga::math::Vector3& axis, float angle)
{
    float half_angle = angle * 0.5f;
    float s = std::sin(half_angle);
    
    return Quaternion(
        std::cos(half_angle),
        axis.x_ * s,
        axis.y_ * s,
        axis.z_ * s
    );
}

void Rotation::RotationMatrixX(float angle, float matrix[9])
{
    float c = std::cos(angle);
    float s = std::sin(angle);
    
    matrix[0] = 1.0f; matrix[1] = 0.0f; matrix[2] = 0.0f;
    matrix[3] = 0.0f; matrix[4] = c;    matrix[5] = -s;
    matrix[6] = 0.0f; matrix[7] = s;    matrix[8] = c;
}

void Rotation::RotationMatrixY(float angle, float matrix[9])
{
    float c = std::cos(angle);
    float s = std::sin(angle);
    
    matrix[0] = c;    matrix[1] = 0.0f; matrix[2] = s;
    matrix[3] = 0.0f; matrix[4] = 1.0f; matrix[5] = 0.0f;
    matrix[6] = -s;   matrix[7] = 0.0f; matrix[8] = c;
}

void Rotation::RotationMatrixZ(float angle, float matrix[9])
{
    float c = std::cos(angle);
    float s = std::sin(angle);
    
    matrix[0] = c;    matrix[1] = -s;   matrix[2] = 0.0f;
    matrix[3] = s;    matrix[4] = c;    matrix[5] = 0.0f;
    matrix[6] = 0.0f; matrix[7] = 0.0f; matrix[8] = 1.0f;
}

makga::math::Vector3 Rotation::RotateByMatrix(const makga::math::Vector3& v, const float matrix[9])
{
    return makga::math::Vector3(
        matrix[0] * v.x_ + matrix[1] * v.y_ + matrix[2] * v.z_,
        matrix[3] * v.x_ + matrix[4] * v.y_ + matrix[5] * v.z_,
        matrix[6] * v.x_ + matrix[7] * v.y_ + matrix[8] * v.z_
    );
}

Rotation::Quaternion Rotation::Slerp(const Quaternion& start, const Quaternion& end, float t)
{
    float dot = start.w * end.w + start.x * end.x + start.y * end.y + start.z * end.z;
    
    Quaternion end_adjusted = end;
    if (dot < 0.0f) {
        end_adjusted.w = -end.w;
        end_adjusted.x = -end.x;
        end_adjusted.y = -end.y;
        end_adjusted.z = -end.z;
        dot = -dot;
    }
    
    if (dot > 0.9995f) {
        // 선형 보간
        return Quaternion(
            start.w + t * (end_adjusted.w - start.w),
            start.x + t * (end_adjusted.x - start.x),
            start.y + t * (end_adjusted.y - start.y),
            start.z + t * (end_adjusted.z - start.z)
        ).Normalize();
    }
    
    float theta_0 = std::acos(dot);
    float theta = theta_0 * t;
    
    float sin_theta = std::sin(theta);
    float sin_theta_0 = std::sin(theta_0);
    
    float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;
    
    return Quaternion(
        s0 * start.w + s1 * end_adjusted.w,
        s0 * start.x + s1 * end_adjusted.x,
        s0 * start.y + s1 * end_adjusted.y,
        s0 * start.z + s1 * end_adjusted.z
    );
}

float Rotation::DegToRad(float degrees)
{
    return degrees * 3.14159265f / 180.0f;
}

float Rotation::RadToDeg(float radians)
{
    return radians * 180.0f / 3.14159265f;
}

// Velocity 구현
makga::math::Vector3 Velocity::AverageVelocity(const makga::math::Vector3& displacement, float time)
{
    if (time > 0.0f)
        return displacement / time;
    return makga::math::Vector3(0.0f, 0.0f, 0.0f);
}

makga::math::Vector3 Velocity::InstantaneousVelocity(const makga::math::Vector3& initial_velocity, const makga::math::Vector3& acceleration, float time)
{
    return initial_velocity + acceleration * time;
}

makga::math::Vector3 Velocity::AverageAcceleration(const makga::math::Vector3& velocity_change, float time)
{
    if (time > 0.0f)
        return velocity_change / time;
    return makga::math::Vector3(0.0f, 0.0f, 0.0f);
}

makga::math::Vector3 Velocity::AngularToLinear(const makga::math::Vector3& angular_velocity, const makga::math::Vector3& radius_vector)
{
    return angular_velocity.Cross(radius_vector);
}

} // namespace makga::math
