module;

export module makga.lib.ecs.component;

export namespace makga::lib {
export class Component
{
public:
	enum Type
	{
		NONE = 0,
		TRANSFORM,
		RIGID_BODY,
		COLLIDER,
	};

	Component(Type type)
		: type_(type) {}
	virtual ~Component() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update() = 0;

	Type GetType() const
	{
		return type_;
	}

protected:
	Type type_;
};
};