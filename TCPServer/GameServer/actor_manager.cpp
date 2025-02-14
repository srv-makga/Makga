#include "pch.h"
#include "actor_manager.h"
#include "actor_character.h"
#include "actor_monster.h"
#include "actor_npc.h"
#include "actor_gadget.h"
#include "actor_pet.h"

ActorManager::ActorManager()
	//: m_characters(0)
	//, m_monsters(100000000)
	//, m_gadgets(200000000)
	//, m_pets(300000000)
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
}

void ActorManager::CreateActor(std::size_t _create_character, std::size_t _create_npc, std::size_t _create_monster, std::size_t _create_gadget, std::size_t _create_pet)
{
	//m_characters.InitPool(_create_character, 50);
	//m_monsters.InitPool(_create_monster, 50);
	//m_gadgets.InitPool(_create_gadget, 50);
	//m_pets.InitPool(_create_pet, 50);
}

std::shared_ptr<Actor> ActorManager::FindActor(ActorUid_t _uid)
{
    return std::shared_ptr<Actor>();
}

std::shared_ptr<Actor> ActorManager::AllocActor(fb::eActorType _type)
{
	// @todo 주석 제거 필요
	//switch (_type)
	//{
	//case eActorType_Monster:
	//	return AllocMonster();
	//case eActorType_Character:
	//	return AllocCharacter();
	//case eActorType_Gadget:
	//	return AllocGadget();
	//case eActorType_Pet:
	//	return AllocPet();
	//}

	return nullptr;
}

std::shared_ptr<Character> ActorManager::AllocCharacter()
{
	return nullptr;
//	return m_characters.Alloc();
}

void ActorManager::DeallocCharacter(std::shared_ptr<Character> _actor)
{
//	m_characters.Dealloc(_actor);
}

std::shared_ptr<Monster> ActorManager::AllocMonster()
{
	return nullptr;
//	return m_monsters.Alloc();
}

void ActorManager::DeallocMonster(std::shared_ptr<Monster> _actor)
{
//	m_monsters.Dealloc(_actor);
}

std::shared_ptr<Gadget> ActorManager::AllocGadget()
{
	return nullptr;
//	return m_gadgets.Alloc();
}

void ActorManager::DeallocGadget(std::shared_ptr<Gadget> _actor)
{
//	m_gadgets.Dealloc(_actor);
}

std::shared_ptr<Pet> ActorManager::AllocPet()
{
	return nullptr;
//	return m_pets.Alloc();
}

void ActorManager::DeallocPet(std::shared_ptr<Pet> _actor)
{
//	m_pets.Dealloc(_actor);
}