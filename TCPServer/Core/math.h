#pragma once

#include "singleton.hpp"

namespace core {
namespace math {
class Math : public core::pattern::Singleton<Math>
{
public:
	Math() = default;
	~Math() = default;

public:
	float Distance(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2);
	float DistanceSquare(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2);
};
} // namespace math
} // namespace core

#define MATH core::math::Math::Instance()