#pragma once

#include "game_header.h"
#include "../Core/object_pool.h"

class Terrain;
class ActorAI;
class Character;
class Monster;
class Npc;
class Gadget;

/*
* @brief 맵에서 보일 객체의 인터페이스
*/
class Actor
{
public:
	Actor() = default;
	virtual ~Actor() = default;

public: // 가상 & 기능
	virtual void Initialize() = 0;
	virtual void Finallize() = 0;

	virtual bool SetTable(ActorBasicTable* _table) = 0;
	virtual void OnUpdate() = 0;

	virtual Result_t DoMove(const PositionT& _position) = 0;

	virtual Result_t Move(Coord_t _x, Coord_t _y, Coord_t _z) = 0;
	virtual Result_t Move(Vector_t _vec) = 0;

	virtual Actor* FindTarget() = 0;
	// @brief 해당 액터를 인지할 수 있는 상태인지
	virtual bool CanSeeActor(Actor* _actor) = 0;

	// @brief 특정 좌표 리스트를 반복적으로 이동하기 위한 설정
	virtual void SetRoute(Sequence_t _seq, const Vector_t& _vec) {}

	// @brief 특정 모습으로 변경
	virtual Result_t Change(TableIdx_t _index) = 0;
	virtual Result_t ChangeRollback() = 0;

	virtual bool CanResurrecton() const { return false; }
	virtual Result_t Resurrecton() const { return eResult_Success; }

	virtual Result_t DoAttack(Actor* _target, SkillIdx_t _skill_idx) = 0;

	virtual void AddAggroList(Actor* _actor) = 0;
	virtual void AddAggroList(const ActorList& _actor) = 0;
	virtual void SelectTarget() = 0;

	virtual flatbuffers::Offset<fb::ActorInfoBase> OffsetActorInfoBase(FB_BUILDER& _fbb) = 0;
	virtual flatbuffers::Offset<fb::ActorInfoDetail> OffsetActorInfoDetail(FB_BUILDER& _fbb) = 0;

protected:
	// @brief 이동할 목표 지점 설정
	virtual Result_t SetMoveTarget(Vector_t _vec) = 0;
	
public: // 가상 & get set
	virtual fb::eActorType Type() const = 0;
	virtual bool IsCharacter() const { return false; }
	virtual bool IsMonster() const { return false; }
	virtual bool IsNpc() const { return false; }
	virtual bool IsGadget() const { return false; }

	// @brief 공격할 수 있는
	virtual bool IsAttackable() const { return false; }
	// @brief 공격받 수 있는
	virtual bool IsAttacked() const { return false; }
	// @brief 이동이 가능한
	virtual bool IsMovable() const { return false; }
	// @brief 누가 주변에 올 때 appear 정보를 줄지
	virtual bool IsAppear() const { return false; }

	virtual bool HasTarget() const { return false; }
	virtual Actor* Target() const { return nullptr; }
	virtual bool IsDead() const { return true; }

	virtual SkillIdx_t SkillIndex() const { return 0; }

	virtual void SetDefaultPosition() { }
	virtual Result_t SetPosition(const PositionT& _pos) { return eResult_Success; }
	virtual const PositionT& Position() const { return s_empty_position; }
	virtual const PositionT& LastRoutePosition() const { return s_empty_position; }
	virtual bool HasNextRoutePosition() const { return false; }
	virtual const PositionT& NextRoutePosition() const { return s_empty_position; }

	virtual Coord_t X() const { return Coord_t(); }
	virtual Coord_t Y() const { return Coord_t(); }
	virtual Coord_t Z() const { return Coord_t(); }

	virtual Coord_t MaxAroundDistance() const { return 0; }
	virtual Coord_t AttackRange() const { return 0; }

	virtual Distance_t MySight() const { return 0.f; };

	virtual fb::eAiType AIType() const { return eAiType_None; }

	virtual Hp_t MaxHp() const { return 0; }
	virtual Hp_t CurHp() const { return 0; }
	virtual Mp_t MaxMp() const { return 0; }
	virtual Mp_t CurMp() const { return 0; }

	virtual Speed_t Speed() const { return 0; }

	virtual Terrain* CurTerrain() const { return nullptr; }

	virtual Actor* Owner() const { return nullptr; }

public: // actor 자체
	ActorUid_t Uid() const { return m_uid; }
	void SetUId(ActorUid_t _uid) { m_uid = _uid; }
	ActorAI* AI() const { return m_ai; }

protected:
	ActorUid_t m_uid;
	ActorAI* m_ai;

	inline static const PositionT s_empty_position;
};