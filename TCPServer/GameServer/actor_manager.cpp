#include "pch.h"
#include "actor_manager.h"
#include "actor_character.h"
#include "actor_monster.h"
#include "actor_npc.h"
#include "actor_gadget.h"
#include "actor_pet.h"

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
		Character::Return(iter.second);
	}

	m_characters.clear();

	for (auto& iter : m_monsters)
	{
		Monster::Return(iter.second);
	}

	m_monsters.clear();

	for (auto& iter : m_npcs)
	{
		Npc::Return(iter.second);
	}

	m_npcs.clear();

	for (auto& iter : m_gadgets)
	{
		Gadget::Return(iter.second);
	}

	m_gadgets.clear();
}

void ActorManager::CreateActor(std::size_t _create_character, std::size_t _create_npc, std::size_t _create_monster, std::size_t _create_gadget, std::size_t _create_pet)
{
	for (std::size_t i = 0; i < _create_character; ++i)
	{
		Character* actor = new Character(++m_actor_id);
		actor->Initialize();
		Character::Return(actor);
	}

	for (std::size_t i = 0; i < _create_npc; ++i)
	{
		Npc* actor = new Npc(++m_actor_id);
		actor->Initialize();
		Npc::Return(actor);
	}

	for (std::size_t i = 0; i < _create_monster; ++i)
	{
		Monster* actor = new Monster(++m_actor_id);
		actor->Initialize();
		Monster::Return(actor);
	}

	for (std::size_t i = 0; i < _create_gadget; ++i)
	{
		Gadget* actor = new Gadget(++m_actor_id);
		actor->Initialize();
		Gadget::Return(actor);
	}

	for (std::size_t i = 0; i < _create_pet; ++i)
	{
		Pet* actor = new Pet(++m_actor_id);
		actor->Initialize();
		Pet::Return(actor);
	}
}

Character* ActorManager::AllocCharacter()
{
	Character* actor = Character::Rental();

	m_characters.insert({ actor->Uid(), actor });
	Add(actor->Uid(), actor);

	return actor;
}

void ActorManager::ReallocCharacter(Character* _actor)
{
	if (nullptr == _actor)
	{
		return;
	}

	m_characters.erase(_actor->Uid());
	Remove(_actor->Uid());

	Character::Return(_actor);
}

Monster* ActorManager::AllocMonster()
{
	Monster* actor = Monster::Rental();

	m_monsters.insert({ actor->Uid(), actor });
	Add(actor->Uid(), actor);

	return actor;
}

void ActorManager::ReallocMonster(Monster* _actor)
{
	if (nullptr == _actor)
	{
		return;
	}

	m_monsters.erase(_actor->Uid());
	Remove(_actor->Uid());

	Monster::Return(_actor);
}

Npc* ActorManager::AllocNpc()
{
	Npc* actor = Npc::Rental();

	m_npcs.insert({ actor->Uid(), actor });
	Add(actor->Uid(), actor);

	return actor;
}

void ActorManager::ReallocNpc(Npc* _actor)
{
	if (nullptr == _actor)
	{
		return;
	}

	m_npcs.erase(_actor->Uid());
	Remove(_actor->Uid());

	Npc::Return(_actor);
}

Gadget* ActorManager::AllocGadget()
{
	Gadget* actor = Gadget::Rental();

	m_gadgets.insert({ actor->Uid(), actor });
	Add(actor->Uid(), actor);

	return actor;
}

void ActorManager::ReallocGadget(Gadget* _actor)
{
	if (nullptr == _actor)
	{
		return;
	}

	m_gadgets.erase(_actor->Uid());
	Remove(_actor->Uid());

	Gadget::Return(_actor);
}
