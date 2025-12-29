module;

export module makga.lib.component.collider;

import makga.math.vector3;
import makga.lib.component;
import makga.lib.component.geometry;

export namespace makga::lib {
/*
Point vs Sphere	점과 구 중심 사이의 거리 < 반지름
Sphere vs Sphere	두 구 중심 사이의 거리 < 두 반지름의 합
Box vs Box (AABB)	각 축(X, Y, Z)의 Min/Max 범위가 모두 겹치는지 확인
Line vs Sphere	선분에서 구 중심과 가장 가까운 점(Projection)을 찾아 거리를 구 반지름과 비교
Sphere vs Box	박스 내에서 구 중심과 가장 가까운 점을 찾고, 그 점과 구 중심의 거리를 비교
Capsule vs Capsule	두 캡슐의 '중심 선분' 사이의 최단 거리를 구한 뒤, (반지름 합)과 비교
*/

export class Collider : public Component
{
public:
	Collider();
	virtual ~Collider() = default;

	bool Initialize() override {}
	void Finalize() override {}
	void Update() override {}

	bool CheckCollision(const Point& g1, const Line& g2, float epsilon = 0.001f);
	bool CheckCollision(const Line& g1, const Point& g2, float epsilon = 0.001f) { return CheckCollision(g2, g1, epsilon); }

	bool CheckCollision(const Point& g1, const Sphere& g2);
	bool CheckCollision(const Sphere& g1, const Point& g2) { return CheckCollision(g2, g1); }

	bool CheckCollision(const Point& g1, const Box& g2);
	bool CheckCollision(const Box& g1, const Point& g2) { return CheckCollision(g2, g1); }

	bool CheckCollision(const Point& g1, const Capsule& g2);
	bool CheckCollision(const Capsule& g1, const Point& g2) { return CheckCollision(g2, g1); }

	bool CheckCollision(const Box& g1, const Sphere& g2);
	bool CheckCollision(const Sphere& g1, const Box& g2) { return CheckCollision(g2, g1); }

	bool CheckCollision(const Sphere& g1, const Capsule& g2);
	bool CheckCollision(const Capsule& g1, const Sphere& g2) { return CheckCollision(g2, g1); }

	bool CheckCollision(const Point& g1, const Point& g2);
	bool CheckCollision(const Box& g1, const Box& g2);
	bool CheckCollision(const Capsule& g1, const Capsule& g2);
	bool CheckCollision(const makga::math::Vector3& g1, const makga::math::Vector3& g2);
};
} // namespace makga::lib