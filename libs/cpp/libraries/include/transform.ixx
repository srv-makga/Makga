module;

export module makga.lib.component.transform;

import makga.math.vector3;
import makga.lib.component;

export namespace makga::lib {
// @detail 위치, 회전, 스케일 정보를 담는 컴포넌트
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