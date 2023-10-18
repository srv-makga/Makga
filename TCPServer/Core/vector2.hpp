#pragma once

#include "core_header.h"
#include <math.h>

namespace core {
namespace coord {
template<typename T>
class Vector2
{
	T m_x;
	T m_y;

public:
	Vector2()
		: m_x(T())
		, m_y(T())
	{
	}

	Vector2(T _x, T _y)
		: m_x(_x)
		, m_y(_y)
	{
	}

	~Vector2() = default;

	void Initialize(T _x, T _y)
	{
		m_x = _x;
		m_y = _y;
	}

	void Clear()
	{
		m_x = 0;
		m_y = 0;
	}

	void operator=(const Vector2& other)
	{
		m_x = other.m_x;
		m_y = other.m_y;
	}

	bool operator==(const Vector2& other)
	{
		return  m_x == other.m_x && m_y == other.m_y;
	}

	Vector2 operator+(const Vector2& other)
	{
		return Vector2(m_x + other.m_x, m_y + other.m_y);
	}

	Vector2 operator-(const Vector2& other)
	{
		return Vector2(m_x - other.m_x, m_y - other.m_y);
	}

	Vector2 operator*(const Vector2& other)
	{
		return Vector2(m_x * other.m_x, m_y * other.m_y);
	}

	Vector2 operator/(const Vector2& other)
	{
		return Vector2(m_x / other.m_x, m_y / other.m_y);
	}

	Vector2& operator+=(const Vector2& other)
	{
		m_x += other.m_x;
		m_y += other.m_y;
		return *this;
	}

	Vector2& operator-=(const Vector2& other)
	{
		m_x -= other.m_x;
		m_y -= other.m_y;
		return *this;
	}

	Vector2& operator*=(const Vector2& other)
	{
		m_x *= other.m_x;
		m_y *= other.m_y;
		return *this;
	}

	Vector2& operator/=(const Vector2& other)
	{
		m_x /= other.m_x;
		m_y /= other.m_y;
		return *this;
	}

	// @brief Vector2 사이의 거리 값(제곱근X)
	T DistanceNotSquareRoot(const Vector2& other)
	{
		return m_x * other.m_x + m_y * other.m_y;
	}

	// @brief Vector2 사이의 거리 값
	T Distance(const Vector2& other)
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			return std::sqrtf(m_x * other.m_x + m_y * other.m_y);
		}

		return std::sqrt(m_x * other.m_x + m_y * other.m_y);
	}

	inline T X() const { return m_x; }
	inline void SetX(T _x) { return m_x = _x; }

	inline T Y() const { return m_y; }
	inline void SetY(T _y) { return m_y = _y; }
};
} // namespace coord
} // namespace core