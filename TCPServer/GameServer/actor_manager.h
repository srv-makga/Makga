#pragma once

#include "actor.h"
#include "../Common/manager_base.hpp"
#include "../Core/object_pool.h"
#include "actor_character.h"
#include "actor_monster.h"
#include "actor_gadget.h"
#include "actor_pet.h"

class ActorManager
{
public:
	template <typename T>
	class ActorData
	{
	public:
		ActorData(ActorUid_t _start_uid) : m_uid(_start_uid) {}
		~ActorData() = default;

		void InitPool(std::size_t _init_size, std::size_t _extend_size)
		{
			m_free.Initialize(_init_size, _extend_size, []() { return std::make_shared<T>(); }, [](std::shared_ptr<T> _object) { _object.reset(); });
		}

		std::shared_ptr<T> Alloc()
		{
			core::WriteLock lock(m_mutex);

			std::shared_ptr<T> ret = m_free.Pop();
			if (ret == nullptr)
			{
				return nullptr;
			}

			ret->SetActorUid(++m_uid);

			if (false == m_use.insert({ ret->ActorUid(), ret }).second)
			{
				ret->Initialize();
				m_free.Push(ret);

				return nullptr;
			}

			return ret;
		}

		void Dealloc(std::shared_ptr<T> _actor)
		{
			if (nullptr == _actor)
			{
				return;
			}

			core::WriteLock lock(m_mutex);
			auto iter = m_use.find(_actor->ActorUid());
			if (m_use.end() == iter)
			{
				return;
			}

			m_use.erase(iter);
			m_free.Push(_actor);
		}

	private:
		core::RWMutex m_mutex;
		ActorUid_t m_uid;
		std::unordered_map<ActorUid_t, std::shared_ptr<T>> m_use;
		core::ObjectPool<std::shared_ptr<T>> m_free;
	};

public:
	using Actors_t = std::unordered_map<ActorUid_t, Actor*>;
public:
	ActorManager();
	virtual ~ActorManager();

	void Initialize();
	void Finalize();

	// @brief 액터의 풀링 설정
	void CreateActor(std::size_t _create_character, std::size_t _create_npc, std::size_t _create_monster, std::size_t _create_gadget, std::size_t _create_pet);

	std::shared_ptr<Actor> FindActor(ActorUid_t _uid);

	std::shared_ptr<Actor> AllocActor(fb::eActorType _type);

	std::shared_ptr<Character> AllocCharacter();
	void DeallocCharacter(std::shared_ptr<Character> _actor);

	std::shared_ptr<Monster> AllocMonster();
	void DeallocMonster(std::shared_ptr<Monster> _actor);

	std::shared_ptr<Gadget> AllocGadget();
	void DeallocGadget(std::shared_ptr<Gadget> _actor);

	std::shared_ptr<Pet> AllocPet();
	void DeallocPet(std::shared_ptr<Pet> _actor);

private:
	//ActorData<Character> m_characters;
	//ActorData<Monster> m_monsters;
	//ActorData<Gadget> m_gadgets;
	//ActorData<Pet> m_pets;
};