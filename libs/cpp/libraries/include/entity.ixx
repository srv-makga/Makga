module;

#include <memory>

export module makga.lib.pattern.entity;

import <tuple>;

export namespace makga::lib {
export template<typename... Components>
class Entity
{
public:
	Entity() = default;
	virtual ~Entity() = default;

	template<typename T>
	T* GetComponent()
	{
		return std::get<std::unique_ptr<T>>(components_).get();
	}

protected:
	std::tuple<std::unique_ptr<Components>...> components_;
};
} // namespace makga::lib