#pragma once

#include "game_header.h"
#include "actor_ai.h"
#include "../Core/object_pool.h"

class Terrain;
class TerrainGrid;
class ActorAI;

// @brief 맵에서 움직일 수 있는 모든 객체의 인터페이스
class Actor : public std::enable_shared_from_this<Actor>
{
public:
	Actor() = default;
	virtual ~Actor() = default;

public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	virtual void SetActorTable(const ActorBasicTable* _table) = 0;
	virtual void OnUpdate() = 0;

public: // 결과가 필요한 함수들
	// @brief 이동이 불가능한 경우 _position가 변경되어 이동될 수 있음
	virtual Result_t DoMove(fb::PositionT& _position) = 0;
	// @brief 이동이 불가능한 경우 실패
	virtual Result_t DoMove(const fb::PositionT& _position) = 0;
	virtual Result_t DoAttack(std::shared_ptr<Actor> _attacked, SkillIdx_t _skill_idx) = 0;
	// @brief 좌표 갱신(Terrain 등 제반 작업 포함)
	virtual Result_t UpdatePosition(const PositionT& _position, fb::eActorMoveEffect _move_effect) = 0;
	// @brief 좌표 갱신(Terrain 등 제반 작업 포함)
	virtual Result_t UpdatePosition(TerrainIdx_t _terrain_idx, const PositionT& _position, fb::eActorMoveEffect _move_effect) = 0;
	// @brief 부활
	virtual Result_t DoResurrecton() = 0;

public:
	virtual void AddAggroList(std::shared_ptr<Actor> _actor) = 0;
	virtual void AddAggroList(const ActorList& _actor) = 0;
	virtual void RemoveAggroList(const std::shared_ptr<Actor> _actor) = 0;

	// @brief 어그로 목록에서 타겟 찾기
	virtual bool SelectTarget() = 0;

	virtual const fb::PositionT& NextRoutePosition() = 0;

//	virtual Result_t DoMove(const PositionT& _position) = 0;
//	virtual Result_t Move(Coord_t _x, Coord_t _y, Coord_t _z) = 0;
//
//	// @breif 상대방에게 나를 지우는 함수
//	virtual void SendDelete(std::shared_ptr<Actor> _to_actor, eActorMoveEffect _effect) = 0;
//	virtual void SendDelete(const std::vector<std::shared_ptr<Actor>>& _to_actors, eActorMoveEffect _effect) = 0;
//	virtual void SendDelete(const std::unordered_set<std::shared_ptr<Actor>>& _to_actors, eActorMoveEffect _effect) = 0;
//
//	// @brief 적 찾기
//	virtual std::shared_ptr<Actor> FindTarget() = 0;
//	
//
//	// @brief 해당 액터를 인지할 수 있는 상태인지
//	virtual bool CanSeeActor(std::shared_ptr<Actor> _actor) = 0;
//
//	virtual void SetRouteTable(TableIdx_t _idx) = 0;
//
//	// @brief 특정 좌표 리스트를 반복적으로 이동하기 위한 설정
//	virtual void SetRoute(Sequence_t _seq, const Vector_t& _vec) = 0;
//
//	// @brief 특정 모습으로 변경
//	virtual Result_t Change(TableIdx_t _index) = 0;
//	virtual Result_t ChangeRollback() = 0;
//

//	virtual Result_t Resurrecton() const = 0;
//
//	virtual Result_t DoAttack(std::shared_ptr<Actor> _target, SkillIdx_t _skill_idx) = 0;
//
//	virtual void StartAI() = 0;
//
//	virtual void SelectTarget() = 0;
//
//#pragma region broadcast & multicast
//public:
//	virtual void MulticastHpMp() = 0;
//	virtual void MulticastBuff() = 0;
//	virtual void Multicast(fb::server::RecvPid, flatbuffers::FlatBufferBuilder& _builder) = 0;
//#pragma endregion
//
//#pragma region flatbuffer_offset 
//public:
//	virtual flatbuffers::Offset<fb::ActorAppear> OffsetActorAppear(FB_BUILDER& _fbb, eActorMoveEffect _effect) = 0;
//	virtual flatbuffers::Offset<fb::ActorDisAppear> OffsetActorDisappear(FB_BUILDER& _fbb, eActorMoveEffect _effect) = 0;
//	virtual flatbuffers::Offset<fb::ActorMove> OffsetActorMove(FB_BUILDER& _fbb) = 0;
//	virtual flatbuffers::Offset<fb::ActorInfoBase> OffsetActorInfoBase(FB_BUILDER& _fbb) = 0;
//	virtual flatbuffers::Offset<fb::ActorInfoDetail> OffsetActorInfoDetail(FB_BUILDER& _fbb) = 0;
//#pragma endregion
//
//protected:
//	// @brief 이동할 목표 지점 설정
//	virtual Result_t SetMoveTarget(Vector_t _vec) = 0;
//	
//
//	virtual SkillIdx_t SkillIndex() const = 0;
//

//	virtual bool HasNextRoutePosition() const = 0;
//	virtual const PositionT& NextRoutePosition() const = 0;
//
//	virtual Coord_t X() const = 0;
//	virtual Coord_t Y() const = 0;
//	virtual Coord_t Z() const = 0;
//
//	virtual Coord_t MaxAroundDistance() const = 0;
//	virtual Coord_t AttackRange() const = 0;
//
//	virtual Distance_t MySight() const = 0;
//	
//	virtual Speed_t MoveBonusSpeed() const = 0;
//	virtual Speed_t MoveSpeed() const = 0;
//
//	virtual fb::eAiType AIType() const = 0;
//
//	virtual Hp_t MaxBonusHp() const = 0;
//	virtual Mp_t MaxBonusMp() const = 0;
//
//	virtual Hp_t MaxHp() const = 0;
//	virtual Hp_t CurHp() const = 0;
//	virtual Mp_t MaxMp() const = 0;
//	virtual Mp_t CurMp() const = 0;
//
//	virtual Terrain* CurTerrain() const = 0;
//	virtual TerrainGrid* CurTerrainGrid() const = 0;
//	virtual void SetTerrainGrid(TerrainGrid* _grid) = 0;
//
//	virtual std::shared_ptr<Actor> Owner() const = 0;
//	virtual std::shared_ptr<User> OwnerUser() const = 0;

public: // is, can => return bool
//	virtual bool IsCharacter() const = 0;
//	virtual bool IsMonster() const = 0;
//	virtual bool IsNpc() const = 0;
//	virtual bool IsGadget() const = 0;
//	virtual bool IsPet() const = 0;
//
//	// @brief 공격할 수 있는
//	virtual bool IsAttackable() const { return false; }
//	// @brief 공격받 수 있는
//	virtual bool IsAttacked() const { return false; }
//	// @brief 이동이 가능한
//	virtual bool IsMovable() const = 0;
//	// @brief 누가 주변에 올 때 appear 정보를 줄지
//	virtual bool IsSendAppear() const = 0;
//	// @brief 사망 상태 여부
	virtual bool IsDead() const = 0;

	virtual bool CanMove() const = 0;
	virtual bool CanResurrecton() const = 0;
	virtual bool HasTarget() const = 0;
	virtual bool HasNextRoutePosition() const = 0;

public: // get set
	virtual ActorUid_t ActorUid() const = 0;
	virtual void SetActorUid(ActorUid_t _uid) = 0;

	virtual fb::eActorType Type() const = 0;
	virtual const ActorBasicTable* Table() const = 0;

	virtual const String8& Name() const = 0;
	virtual std::shared_ptr<ActorAI> GetActorAI() const = 0;
	virtual fb::eAiType AIType() const = 0;

	virtual Terrain* GetTerrain() const = 0;

	virtual TerrainGrid* GetTerrainGrid() const = 0;
	virtual void SetTerrainGrid(TerrainGrid* _grid) = 0;

	virtual std::shared_ptr<Actor> Target() const = 0;
	virtual void SetTarget(std::shared_ptr<Actor> _actor) const = 0;

	virtual const PositionT& Position() const = 0;
	virtual void SetPosition(const PositionT& _pos) = 0;

	// @brief 스폰 위치(캐릭터 - 부활	위치, 몬스터 - 스폰 위치)
	virtual const PositionT& SpawnPosition() const = 0;
	virtual void SetSpawnPosition(const PositionT& _pos) = 0;

	virtual SkillIdx_t SkillIndex() const = 0;

	// @brief 스폰 지점으로부터 최대 움직일  수 있는 거리
	virtual Distance_t MaxAroundDistance() const = 0;
	// @brief 공격 가능한 거리
	// @details 현재 스킬이 있다면 스킬의 사정거리, 없다면 액터의 기본거리
	virtual Distance_t AttackRange() const = 0;

	virtual Angle_t Angle() const = 0;
	virtual void SetAngle(Angle_t _angle) = 0;

	virtual std::shared_ptr<Actor> Owner() const = 0;
	virtual std::shared_ptr<User> OwnerUser() const = 0;
};