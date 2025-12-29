module;

export module makga.math.vector3;

export namespace makga::math {
export class Vector3
{
public:
	Vector3();
	Vector3(float x, float y, float z);
	virtual ~Vector3();

	// @brief ∫§≈Õ ∞£¿« ¡¶∞ˆ ∞≈∏Æ
	float SquareDistance(const Vector3& other) const;
	// @brief ∫§≈Õ ∞£¿« ∞≈∏Æ
	float Distance(const Vector3& other) const;
	// @brief ∫§≈Õ ≥ª¿˚
	float Dot(const Vector3& other) const;
	// @brief ∫§≈Õ ø‹¿˚
	Vector3 Cross(const Vector3& other) const;
	// @brief ∫§≈Õ ¡§±‘»≠
	Vector3 ClosestPointOnLineSegment(const Vector3& a, const Vector3& b) const;

	/*ClosestDistanceSquaredBetweenLineSegments*/
	Vector3 ClosestDistanceSquaredBetweenLineSegments(const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2, float& s, float& t) const;


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
};