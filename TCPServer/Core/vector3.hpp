#include <math.h>

namespace core {
namespace coord {
template<typename T>
class Vector3
{
	T m_x;
	T m_y;
	T m_z;

public:
	Vector3()
		: m_x(T())
		, m_y(T())
		, m_z(T())
	{
	}

	Vector3(T _x, T _y, T _z)
		: m_x(_x)
		, m_y(_y)
		, m_z(_z)
	{
	}

	~Vector3() = default;

	void Initialize(T _x, T _y, T _z)
	{
		m_x = _x;
		m_y = _y;
		m_z = _z;
	}

	void clear()
	{
		m_x = 0;
		m_y = 0;
		m_z = 0;
	}

	void operator=(const Vector3& other_vector3)
	{
		m_x = other_vector3.X();
		m_y = other_vector3.Y();
		m_z = other_vector3.Z();
	}

	bool operator==(const Vector3& other_vector3)
	{
		return X() == other_vector3.X() &&
			Y() == other_vector3.Y() &&
			Z() == other_vector3.Z();
	}

	Vector3 operator+(const Vector3& other_vector3)
	{
		return Vector3(X() + other_vector3.X(), Y() + other_vector3.Y(), Z() + other_vector3.Z());
	}

	Vector3 operator-(const Vector3& other_vector3)
	{
		return Vector3(X() - other_vector3.X(), Y() - other_vector3.Y(), Z() - other_vector3.Z());
	}

	Vector3 operator*(const Vector3& other_vector3)
	{
		return Vector3(X() * other_vector3.X(), Y() * other_vector3.Y(), Z() * other_vector3.Z());
	}

	Vector3 operator/(const Vector3& other_vector3)
	{
		return Vector3(X() / other_vector3.X(), Y() / other_vector3.Y(), Z() / other_vector3.Z());
	}

	Vector3 operator*(T val)
	{
		return Vector3(X() * val, Y() * val, Z() * val);
	}

	Vector3& operator+=(const Vector3& other_vector3)
	{
		m_x += other_vector3.X();
		m_y += other_vector3.Y();
		m_z += other_vector3.Z();

		return *this;
	}

	Vector3& operator-=(const Vector3& other_vector3)
	{
		m_x -= other_vector3.X();
		m_y -= other_vector3.Y();
		m_z -= other_vector3.Z();

		return *this;
	}

	Vector3& operator*=(const Vector3& other_vector3)
	{
		m_x *= other_vector3.X();
		m_y *= other_vector3.Y();
		m_z *= other_vector3.Z();

		return *this;
	}

	Vector3& operator/=(const Vector3& other_vector3)
	{
		m_x /= other_vector3.X();
		m_y /= other_vector3.Y();
		m_z /= other_vector3.Z();

		return *this;
	}

	// @brief 거리 계산 후 제곱근X
	T NormalizationNotSquareRoot() const
	{
		return (m_x * m_x) + (m_y * m_y) + (m_z * m_z);
	}

	// @brief 벡터의 크기 구하기
	float Normalization()
	{
		if constexpr (std::is_floating_point_v<T>)
		{
			return std::sqrtf(NormalizationNotSquareRoot());
		}

		return std::sqrt(NormalizationNotSquareRoot());
	}

	// 내적
	T InnerProduct(const Vector3& other_vector3)
	{
		return
			(X() * other_vector3.X()) +
			(Y() * other_vector3.Y()) +
			(Z() * other_vector3.Z());
	}

	Vector3 CrossProduct(const Vector3& other_vector3)
	{
		return Vector3(
			Y() * other_vector3.Z() - Z() * other_vector3.Y(),
			Z() * other_vector3.X() - X() * other_vector3.Z(),
			X() * other_vector3.Y() - Y() * other_vector3.X()
		);
	}

	inline T X() const { return m_x; }
	inline void SetX(T _x) { m_x = _x; }

	inline T Y() const { return m_y; }
	inline void SetY(T _y) { m_y = _y; }

	inline T Z() const { return m_z; }
	inline void SetZ(T _z) { m_z = _z; }
};
} // namespace coord
} // namespace core

/*
* 선형 보간법
* Lerp(a,b,f) = (1-f).a + f.b
* a = 시작지점
* b = 도착지점
* f = a에서 b까지를 [0,1] 범위로 환산했을 때 a에서 b 사이의 위치를 나타내는 분수 값
*/