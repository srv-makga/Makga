module;

export module makga.lib.ecs.component.collider;

import makga.math.vector3;
import makga.lib.ecs.component;
import makga.lib.ecs.component.geometry;

export namespace makga::lib {
/*
Point vs Sphere	占쏙옙占쏙옙 占쏙옙 占쌩쏙옙 占쏙옙占쏙옙占쏙옙 占신몌옙 < 占쏙옙占쏙옙占쏙옙
Sphere vs Sphere	占쏙옙 占쏙옙 占쌩쏙옙 占쏙옙占쏙옙占쏙옙 占신몌옙 < 占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙
Box vs Box (AABB)	占쏙옙 占쏙옙(X, Y, Z)占쏙옙 Min/Max 占쏙옙占쏙옙占쏙옙 占쏙옙占?占쏙옙치占쏙옙占쏙옙 확占쏙옙
Line vs Sphere	占쏙옙占싻울옙占쏙옙 占쏙옙 占쌩심곤옙 占쏙옙占쏙옙 占쏙옙占쏙옙占?占쏙옙(Projection)占쏙옙 찾占쏙옙 占신몌옙占쏙옙 占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙
Sphere vs Box	占쌘쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙 占쌩심곤옙 占쏙옙占쏙옙 占쏙옙占쏙옙占?占쏙옙占쏙옙 찾占쏙옙, 占쏙옙 占쏙옙占쏙옙 占쏙옙 占쌩쏙옙占쏙옙 占신몌옙占쏙옙 占쏙옙
Capsule vs Capsule	占쏙옙 캡占쏙옙占쏙옙 '占쌩쏙옙 占쏙옙占쏙옙' 占쏙옙占쏙옙占쏙옙 占쌍댐옙 占신몌옙占쏙옙 占쏙옙占쏙옙 占쏙옙, (占쏙옙占쏙옙占쏙옙 占쏙옙)占쏙옙 占쏙옙
*/

/*
* Point
* Line
* Sphere
* Box
* Capsule
*/

export class Collider : public Component
{
public:
	Collider();
	virtual ~Collider() = default;

	bool Initialize() override { return true; }
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

	bool CheckCollision(const Line& g1, const Sphere& g2);
	bool CheckCollision(const Sphere& g1, const Line& g2) { return CheckCollision(g2, g1); }

	bool CheckCollision(const Line& g1, const Box& g2);
	bool CheckCollision(const Box& g1, const Line& g2) { return CheckCollision(g2, g1); }

	bool CheckCollision(const Line& g1, const Capsule& g2);
	bool CheckCollision(const Capsule& g1, const Line& g2) { return CheckCollision(g2, g1); }

	bool CheckCollision(const Sphere& g1, const Capsule& g2);
	bool CheckCollision(const Capsule& g1, const Sphere& g2) { return CheckCollision(g2, g1); }

	bool CheckCollision(const Sphere& g1, const Box& g2) { return CheckCollision(g2, g1); }
	bool CheckCollision(const Box& g1, const Sphere& g2);

	bool CheckCollision(const Box& g1, const Capsule& g2);
	bool CheckCollision(const Capsule& g1, const Box& g2) { return CheckCollision(g2, g1); }


	bool CheckCollision(const Point& g1, const Point& g2);
	bool CheckCollision(const Box& g1, const Box& g2);
	bool CheckCollision(const Capsule& g1, const Capsule& g2);
	bool CheckCollision(const makga::math::Vector3& g1, const makga::math::Vector3& g2);
};
} // namespace makga::lib