module;

export module makga.game.rigid_body;

import makga.math.vector3;
import makga.game.component;
import makga.game.transform_component;

export namespace makga::game {
export class RigidBody : public Component
{
public:
	RigidBody();
	virtual ~RigidBody();

	bool Initialize() override;
	void Finalize() override;
	void Update() override;

	void FixedUpdate(TransformComponent& transform, float delta_time);

protected:
	makga::math::Vector3 velocity_; // 속도
	float mass_; // 질량
	bool use_gravity_; // 중력 사용 여부
}