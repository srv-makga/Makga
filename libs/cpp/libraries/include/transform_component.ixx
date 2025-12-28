module;

export module makga.game.transform_component;

import makga.game.component;
import makga.math.vector3;

export namespace makga::game {
// @detail 위치, 회전, 스케일 정보를 담는 컴포넌트
export class TransformComponent : public Component
{
public:
	TransformComponent() = default;
	virtual ~TransformComponent() = default;

	bool Initialize() override { return true; }
	void Finalize() override {}
	void Update() override {}

	void SetPosition(float x, float y, float z)
	{
		position_.x_ = x;
		position_.y_ = y;
		position_.z_ = z;
	}

public:
	makga::math::Vector3 position_;
	makga::math::Vector3 rotation_;
	makga::math::Vector3 scale_;
};
} // namespace makga::game