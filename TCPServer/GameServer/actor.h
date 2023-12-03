#pragma once

#include "game_header.h"
#include "../Core/object_pool.h"

class Character;
class Monster;
class Npc;
class Gadget;
class ActorAI;

/*
* @brief �ʿ��� ���� ��ü�� �������̽�
*/
class Actor
{
public:
	Actor() = default;
	virtual ~Actor() = default;

public: // ���� & ���
	virtual void Initialize() = 0;
	virtual void Finallize() = 0;

	virtual bool SetTable(ActorBasicTable* _table) = 0;

	virtual void OnUpdate() = 0;
	virtual Result_t Move(Coord_t _x, Coord_t _y, Coord_t _z) = 0;
	virtual Result_t Move(Vector_t _vec) = 0;

	// @brief Ư�� ��ǥ ����Ʈ�� �ݺ������� �̵��ϱ� ���� ����
	virtual void SetRoute(Sequence_t _seq, const Vector_t& _vec) {}

	// @brief Ư�� ������� ����
	virtual Result_t Change(TableIdx_t _index);
	virtual Result_t ChangeRollback();

	virtual flatbuffers::Offset<fb::ActorInfoBase> OffsetActorInfoBase(FB_BUILDER& _fbb) = 0;
	virtual flatbuffers::Offset<fb::ActorInfoDetail> OffsetActorInfoDetail(FB_BUILDER& _fbb) = 0;
	
public: // ���� & get set
	virtual fb::eActorType Type() = 0;
	virtual bool IsCharacter() const { return false; }
	virtual bool IsMonster() const { return false; }
	virtual bool IsNpc() const { return false; }
	virtual bool IsGadget() const { return false; }

	// @brief ������ �� �ִ�
	virtual bool IsAttackable() const { return false; }
	// @brief ���ݹ� �� �ִ�
	virtual bool IsAttacked() const { return false; }
	// @brief �̵��� ������
	virtual bool IsMovable() const { return false; }
	// @brief ���� �ֺ��� �� �� appear ������ ����
	virtual bool IsAppear() const { return false; }

	virtual bool HasTarget() const { return false; }
	virtual Actor* Target() const { return nullptr; }
	virtual bool IsDie() const { return true; }

	virtual Coord_t X() const { return Coord_t(); }
	virtual Coord_t Y() const { return Coord_t(); }
	virtual Coord_t Z() const { return Coord_t(); }

	virtual Hp_t MaxHp() const { return 0; }
	virtual Hp_t CurHp() const { return 0; }
	virtual Mp_t MaxMp() const { return 0; }
	virtual Mp_t CurMp() const { return 0; }

	virtual Speed_t Speed() const { return 0; }

public: // ���
	ActorUid_t Uid() const { return m_uid; }
	void SetUId(ActorUid_t _uid) { m_uid = _uid; }

protected:
	ActorUid_t m_uid;
	ActorAI* m_ai;
};