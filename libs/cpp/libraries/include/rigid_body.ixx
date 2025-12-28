module;

export module makga.lib.component.rigid_body;

import makga.math.vector3;
import makga.lib.component;
import makga.lib.component.transform;

export namespace makga::lib {
export class RigidBody : public Component
{
public:
	RigidBody();
	virtual ~RigidBody();

	bool Initialize() override;
	void Finalize() override;
	void Update() override;

	void FixedUpdate(TransformComponent& transform, float delta_time);
	void AddForce(const makga::math::Vector3& force);

protected:
	makga::math::Vector3 velocity_; // 속도
	float mass_; // 질량
	bool use_gravity_; // 중력 사용 여부
};
} // namespace makga::lib