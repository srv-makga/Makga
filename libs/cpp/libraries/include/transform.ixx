module;

export module makga.lib.ecs.component.transform;

import makga.math.vector3;
import makga.lib.ecs.component;

export namespace makga::lib {
// @detail 占쏙옙치, 회占쏙옙, 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占?占쏙옙占쏙옙占쏙옙트
export class TransformComponent : public Component
{
public:
	TransformComponent();
	virtual ~TransformComponent() = default;

	bool Initialize() override { return true; }
	void Finalize() override {}
	void Update() override {}

	void SetPosition(float x, float y, float z);

public:
	makga::math::Vector3 position_;
	makga::math::Vector3 rotation_;
	makga::math::Vector3 scale_;
};
} // namespace makga::lib