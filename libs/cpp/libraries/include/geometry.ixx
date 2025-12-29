module;

export module makga.lib.component.geometry;

import makga.math.vector3;

export namespace makga::lib {
export struct Point
{
	makga::math::Vector3 position;
};

export struct Line
{
	makga::math::Vector3 start;
	makga::math::Vector3 end;
};

export struct Sphere
{
	makga::math::Vector3 center;
	float radius;
};

export struct Box
{
	makga::math::Vector3 min; // 최소 좌표 (왼쪽 아래 앞)
	makga::math::Vector3 max; // 최대 좌표 (오른쪽 위 뒤)
};

export struct Capsule
{
	makga::math::Vector3 start; // 캡슐의 한 끝점
	makga::math::Vector3 end;   // 캡슐의 다른 끝점
	float radius;               // 반지름
};
} // namespace makga::lib