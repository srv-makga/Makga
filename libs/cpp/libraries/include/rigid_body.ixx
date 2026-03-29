癤퓅odule;

export module makga.lib.ecs.component.rigid_body;

import makga.math.vector3;
import makga.lib.ecs.component;
import makga.lib.ecs.component.transform;

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
	makga::math::Vector3 velocity_; // 占쌈듸옙
	float mass_; // 占쏙옙占쏙옙
	bool use_gravity_; // 占쌩뤄옙 占쏙옙占?占쏙옙占쏙옙
};
} // namespace makga::lib