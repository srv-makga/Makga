#pragma once

#include "actor.h"
#include "../Common/manager_base.hpp"
#include "../Core/singleton.hpp"

class ActorManager : public ManagerBase<ActorId_t, Actor*>, public core::pattern::Singleton<ActorManager>
{
public:
	using Actors_t = std::unordered_map<ActorId_t, Actor*>;
public:
	ActorManager();
	virtual ~ActorManager();

	void Initialize();
	void Finalize();

	// @brief 액터의 풀링 설정
	void CreateActor(std::size_t _create_character, std::size_t _create_npc, std::size_t _create_monster, std::size_t _create_object);

	Character* AllocCharacter();
	void ReallocCharacter(Character* _actor);

	Monster* AllocMonster();
	void ReallocMonster(Monster* _actor);

	Npc* AllocNpc();
	void ReallocNpc(Npc* _actor);

	Gadget* AllocGadget();
	void ReallocGadget(Gadget* _actor);

private:
	// @detail 사용중인 액터 목록
	std::unordered_map<ActorId_t, Character*> m_characters;
	std::unordered_map<ActorId_t, Monster*> m_monsters;
	std::unordered_map<ActorId_t, Npc*> m_npcs;
	std::unordered_map<ActorId_t, Gadget*> m_gadgets;

	ActorId_t m_actor_id;
};

#define ACTOR_MANAGER	ActorManager::Instance()