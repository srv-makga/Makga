module;

export module makga.math.vector3;

export namespace makga::math {
export class Vector3
{
public:
	Vector3();
	Vector3(float x, float y, float z);
	virtual ~Vector3();

	// @brief 정규화
	void Normalize();
	// @brief 벡터의 길이
	float Length() const;
	// @brief 벡터 간의 제곱 거리
	float SquareDistance(const Vector3& other) const;
	// @brief 벡터 간의 거리
	float Distance(const Vector3& other) const;
	// @brief 벡터 내적
	float Dot(const Vector3& other) const;
	// @brief 벡터 외적
	Vector3 Cross(const Vector3& other) const;
	// @brief 벡터 정규화
	Vector3 ClosestPointOnLineSegment(const Vector3& a, const Vector3& b) const;
	/*ClosestDistanceSquaredBetweenLineSegments*/
	Vector3 ClosestDistanceSquaredBetweenLineSegments(const Vector3& end, const Vector3& other_start, const Vector3& other_end, float& s, float& t) const;


	Vector3 operator+(const Vector3& other) const;
	Vector3 operator-(const Vector3& other) const;
	Vector3 operator*(float scalar) const;
	Vector3 operator/(float scalar) const;

	float GetX() const;
	float GetY() const;
	float GetZ() const;

public:
	float x_;
	float y_;
	float z_;
};
}