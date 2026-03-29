癤퓅odule;

export module makga.lib.ecs.component.geometry;

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
	makga::math::Vector3 min; // 占쌍쇽옙 占쏙옙표 (占쏙옙占쏙옙 占싣뤄옙 占쏙옙)
	makga::math::Vector3 max; // 占쌍댐옙 占쏙옙표 (占쏙옙占쏙옙占쏙옙 占쏙옙 占쏙옙)
};

export struct Capsule
{
	makga::math::Vector3 start; // 캡占쏙옙占쏙옙 占쏙옙 占쏙옙占쏙옙
	makga::math::Vector3 end;   // 캡占쏙옙占쏙옙 占쌕몌옙 占쏙옙占쏙옙
	float radius;               // 占쏙옙占쏙옙占쏙옙
};
} // namespace makga::lib