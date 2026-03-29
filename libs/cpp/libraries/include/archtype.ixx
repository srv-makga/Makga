癤퓅odule;

export module makga.lib.ecs.archtype;

import <vector>;
import <tuple>;

export namespace makga::lib {
export template<typename... Components>
class ArchType
{
public:
	using Chunks = std::tuple<std::vector<Components>...>;

	ArchType() = default;
	virtual ~ArchType() = default;

	void AddEntity(Components... comps)
	{
		AddToTuple(std::forward<Components>(comps)..., std::index_sequence_for<Components...>{});
		++entity_count_;
	}

	template<typename Func>
	void Foreach(Func&& func)
	{
		for (size_t i = 0; i < entity_count_; ++i)
		{
			ApplyAt(i, std::forward<Func>(func), std::index_sequence_for<Components...>{});
		}
	}

	static constexpr size_t GetComponentCount()
	{
		return sizeof...(Components);
	}

	template<typename T>
	bool HasComponent() const
	{
		return std::disjunction_v<std::is_same<T, Components>...>;
	}

	template<typename T>
	T& GetComponent()
	{
		return std::get<T>(chunks_);
	}

private:
	// 튜占쏙옙占쏙옙 占쏙옙 占쏙옙占싶울옙 占쏙옙占쏙옙占싶몌옙 占쌍댐옙 占쏙옙占쏙옙
	template <std::size_t... Is>
	void AddToTuple(Components&&... comps, std::index_sequence<Is...>)
	{
		(std::get<Is>(chunks_).push_back(std::forward<Components>(comps)), ...);
	}

	// 특占쏙옙 占싸듸옙占쏙옙占쏙옙 占쏙옙占쏙옙占싶듸옙占쏙옙 占쌉쇽옙 占쏙옙占쌘뤄옙 占쏙옙占쏙옙占싹댐옙 占쏙옙占쏙옙
	template <typename Func, std::size_t... Is>
	void ApplyAt(size_t index, Func&& func, std::index_sequence<Is...>)
	{
		func(std::get<Is>(chunks_)[index]...);
	}

protected:
	Chunks chunks_;
	size_t entity_count_ = 0;
};
} // namespace makga::lib