module;

#include <memory>

export module makga.lib.component.collider.sphere;

import makga.math.vector3;
import makga.lib.component.collider;
import makga.lib.component.transform;

export namespace makga::lib {
export class SphereCollider : public Collider
{
public:
	SphereCollider(std::shared_ptr<TransformComponent> transform, float radius);
	virtual ~SphereCollider();

	bool Initialize() override;
	void Finalize() override;
	void Update() override {}

	bool CheckCollision(const Collider* other) const override;
	bool CheckCollision(const PointCollider& other) const override;
	bool CheckCollision(const LineCollider& other) const override;
	bool CheckCollision(const SphereCollider& other) const override;
	bool CheckCollision(const BoxCollider& other) const override;
	bool CheckCollision(const CapsuleCollider& other) const override;

	float GetRadius() const;
	const makga::math::Vector3& GetPosition() const;

protected:
	float radius_;
	std::shared_ptr<TransformComponent> transform_;
};
} // namespace makga::lib