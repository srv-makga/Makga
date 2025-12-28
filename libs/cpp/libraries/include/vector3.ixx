module;

export module makga.math.vector3;

export namespace makga::math {
export class Vector3
{
public:
	Vector3();
	Vector3(float x, float y, float z);
	virtual ~Vector3();

	float SquareDistance(const Vector3& other) const;
	float Distance(const Vector3& other) const;
	
	float Dot(const Vector3& other) const;

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