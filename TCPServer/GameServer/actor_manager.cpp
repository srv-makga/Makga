#include "pch.h"
#include "actor_manager.h"
#include "actor_character.h"
#include "actor_monster.h"
#include "actor_npc.h"
#include "actor_gadget.h"

ActorManager::ActorManager()
	: m_actor_id(0)
{
}

ActorManager::~ActorManager()
{
}

void ActorManager::Initialize()
{
}

void ActorManager::Finalize()
{
	for (auto& iter : m_characters)
	{
		POOL(Character)::Push(iter.second);
	}

	m_characters.clear();

	for (auto& iter : m_monsters)
	{
		POOL(Monster)::Push(iter.second);
	}

	m_monsters.clear();

	for (auto& iter : m_npcs)
	{
		POOL(Npc)::Push(iter.second);
	}

	m_npcs.clear();

	for (auto& iter : m_gadgets)
	{
		POOL(Gadget)::Push(iter.second);
	}

	m_gadgets.clear();
}

void ActorManager::CreateActor(std::size_t _create_character, std::size_t _create_npc, std::size_t _create_monster, std::size_t _create_object)
{
	for (std::size_t i = 0; i < _create_character; ++i)
	{
		Character* character = new Character();
		character->Initialize();
		character->SetId(++m_actor_id);

		POOL(Character)::Push(character);
	}

	for (std::size_t i = 0; i < _create_npc; ++i)
	{
		Npc* npc = new Npc();
		npc->Initialize();
		npc->SetId(++m_actor_id);

		POOL(Npc)::Push(npc);
	}

	for (std::size_t i = 0; i < _create_monster; ++i)
	{
		Monster* monster = new Monster();
		monster->Initialize();
		monster->SetId(++m_actor_id);

		POOL(Monster)::Push(monster);
	}

	for (std::size_t i = 0; i < _create_object; ++i)
	{
		Gadget* object = new Gadget();
		object->Initialize();
		object->SetId(++m_actor_id);

		POOL(Gadget)::Push(object);
	}
}

Character* ActorManager::AllocCharacter()
{
	Character* actor = POOL(Character)::Pop();

	m_characters.insert({ actor->Id(), actor });
	Add(actor->Id(), actor);

	return actor;
}

void ActorManager::ReallocCharacter(Character* _actor)
{
	if (nullptr == _actor)
	{
		return;
	}

	m_characters.erase(_actor->Id());
	Remove(_actor->Id());

	POOL(Character)::Push(_actor);
}

Monster* ActorManager::AllocMonster()
{
	Monster* actor = POOL(Monster)::Pop();

	m_monsters.insert({ actor->Id(), actor });
	Add(actor->Id(), actor);

	return actor;
}

void ActorManager::ReallocMonster(Monster* _actor)
{
	if (nullptr == _actor)
	{
		return;
	}

	m_monsters.erase(_actor->Id());
	Remove(_actor->Id());

	POOL(Monster)::Push(_actor);
}

Npc* ActorManager::AllocNpc()
{
	Npc* actor = POOL(Npc)::Pop();

	m_npcs.insert({ actor->Id(), actor });
	Add(actor->Id(), actor);

	return actor;
}

void ActorManager::ReallocNpc(Npc* _actor)
{
	if (nullptr == _actor)
	{
		return;
	}

	m_npcs.erase(_actor->Id());
	Remove(_actor->Id());

	POOL(Npc)::Push(_actor);
}

Gadget* ActorManager::AllocGadget()
{
	Gadget* actor = POOL(Gadget)::Pop();

	m_objects.insert({ actor->Id(), actor });
	Add(actor->Id(), actor);

	return actor;
}

void ActorManager::ReallocGadget(Gadget* _actor)
{
	if (nullptr == _actor)
	{
		return;
	}

	m_objects.erase(_actor->Id());
	Remove(_actor->Id());

	POOL(Gadget)::Push(_actor);
}
