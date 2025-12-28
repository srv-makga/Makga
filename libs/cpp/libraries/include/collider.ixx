module;

export module makga.lib.component.collider;

import makga.math.vector3;
import makga.lib.component;
import makga.lib.component.transform;

export namespace makga::lib {
export enum ColliderType
{
	COLLIDER = 0,
	POINT, // 좌표
	LINE, // 선
	SPHERE, // 구체
	BOX, // 박스
	CAPSULE, // 캡슐
};

/*
Point vs Sphere	점과 구 중심 사이의 거리 < 반지름
Sphere vs Sphere	두 구 중심 사이의 거리 < 두 반지름의 합
Box vs Box (AABB)	각 축(X, Y, Z)의 Min/Max 범위가 모두 겹치는지 확인
Line vs Sphere	선분에서 구 중심과 가장 가까운 점(Projection)을 찾아 거리를 구 반지름과 비교
Sphere vs Box	박스 내에서 구 중심과 가장 가까운 점을 찾고, 그 점과 구 중심의 거리를 비교
Capsule vs Capsule	두 캡슐의 '중심 선분' 사이의 최단 거리를 구한 뒤, (반지름 합)과 비교
*/

class PointCollider;
class LineCollider;
class SphereCollider;
class BoxCollider;
class CapsuleCollider;

export class Collider : public Component
{
public:
	Collider(ColliderType collider_type)
		: Component(COLLIDER)
		, collider_type_(collider_type)
	{
	}
	virtual ~Collider() = default;

	bool Initialize() override {}
	void Finalize() override {}
	void Update() override {}

	virtual bool CheckCollision(const Collider* other) const = 0;
	virtual bool CheckCollision(const PointCollider& other) const = 0;
	virtual bool CheckCollision(const LineCollider& other) const = 0;
	virtual bool CheckCollision(const SphereCollider& other) const = 0;
	virtual bool CheckCollision(const BoxCollider& other) const = 0;
	virtual bool CheckCollision(const CapsuleCollider& other) const = 0;

	ColliderType GetColliderType() const
	{
		return collider_type_;
	}

protected:
	ColliderType collider_type_;
};
} // namespace makga::lib