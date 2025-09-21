#pragma once

#include "game_header.h"
#include "actor_ai.h"
#include "../Core/object_pool.h"

class Terrain;
class TerrainGrid;
class ActorAI;

// @brief �ʿ� �߰��Ǵ� ��� ��ü�� �������̽�
class Actor : public std::enable_shared_from_this<Actor>
{
public:
	using Ptr = std::shared_ptr<Actor>;

public:
	Actor() = default;
	virtual ~Actor() = default;

public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void OnUpdate() = 0;

public: // ����� �ʿ��� �Լ��� (Do�� ����)
	// @brief �̵��� �Ұ����� ��� _position�� ����Ǿ� �̵��� �� ����
	virtual Result_t DoMove(fb::PositionT& _position) = 0;
	// @brief �̵��� �Ұ����� ��� ����
	virtual Result_t DoMove(const fb::PositionT& _position) = 0;
	// @breif ���� �õ�
	// @param cast_pos ��ų ���� ��ġ
	// @detail targetlist�� ���ο��� ����� �� ����
	virtual Result_t DoAttack(SkillIndex_t _index, SkillLevel_t _level, const PositionT& _cast_pos, OUT ActorList& _target_list) = 0; // ��ų ������ �ʿ�
	// @brief ��ǥ ����(Terrain �� ���� �۾� ����)
	virtual Result_t UpdatePosition(const PositionT& _position, fb::eActorMoveEffect _move_effect) = 0;
	// @brief ��ǥ ����(Terrain �� ���� �۾� ����)
	virtual Result_t UpdatePosition(TerrainIdx_t _terrain_idx, const PositionT& _position, fb::eActorMoveEffect _move_effect) = 0;
	// @brief ��Ȱ
	virtual Result_t DoResurrecton() = 0;
	// @brief ���� ��ȣ �ۿ� �õ�
	virtual Result_t DoInteractionStart(std::shared_ptr<Actor> _other) = 0;
	virtual Result_t DoInteraction(std::shared_ptr<Actor> _other) = 0;
	virtual Result_t DoInteractionCancel(std::shared_ptr<Actor> _other) = 0;

public:
	virtual bool IsEnermy(std::shared_ptr<Actor>) = 0;

	virtual void ApplyAbility(const SkillAbility& _ability, ActorUid_t _by_uid, SkillIndex_t _skill_index, Time_t _expireTime) = 0;

	// ��׷�
	virtual void AddAggroList(std::shared_ptr<Actor> _actor) = 0;
	virtual void AddAggroList(const ActorList& _actor) = 0;
	virtual void RemoveAggroList(const std::shared_ptr<Actor> _actor) = 0;

	// @brief ��׷� ��Ͽ��� Ÿ�� ã��
	virtual bool SelectTarget() = 0;

	virtual const fb::PositionT& NextRoutePosition() = 0;

//	virtual Result_t DoMove(const PositionT& _position) = 0;
//	virtual Result_t Move(Coord_t _x, Coord_t _y, Coord_t _z) = 0;
//
//	// @breif ���濡�� ���� ����� �Լ�
//	virtual void SendDelete(std::shared_ptr<Actor> _to_actor, eActorMoveEffect _effect) = 0;
//	virtual void SendDelete(const std::vector<std::shared_ptr<Actor>>& _to_actors, eActorMoveEffect _effect) = 0;
//	virtual void SendDelete(const std::unordered_set<std::shared_ptr<Actor>>& _to_actors, eActorMoveEffect _effect) = 0;
//
//	// @brief �� ã��
//	virtual std::shared_ptr<Actor> FindTarget() = 0;
//	
//
//	// @brief �ش� ���͸� ������ �� �ִ� ��������
//	virtual bool CanSeeActor(std::shared_ptr<Actor> _actor) = 0;
//
//	virtual void SetRouteTable(TableIdx_t _idx) = 0;
//
//	// @brief Ư�� ��ǥ ����Ʈ�� �ݺ������� �̵��ϱ� ���� ����
//	virtual void SetRoute(Sequence_t _seq, const Vector_t& _vec) = 0;
//
//	// @brief Ư�� ������� ����
//	virtual Result_t Change(TableIdx_t _index) = 0;
//	virtual Result_t ChangeRollback() = 0;
//

//	virtual Result_t Resurrecton() const = 0;
//
//	virtual Result_t DoAttack(std::shared_ptr<Actor> _target, SkillIndex_t _skill_idx) = 0;
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
//	// @brief �̵��� ��ǥ ���� ����
//	virtual Result_t SetMoveTarget(Vector_t _vec) = 0;
//	
//
//	virtual SkillIndex_t SkillIndex() const = 0;
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
//	// @brief ������ �� �ִ�
//	virtual bool IsAttackable() const { return false; }
//	// @brief ���ݹ� �� �ִ�
//	virtual bool IsAttacked() const { return false; }
//	// @brief �̵��� ������
//	virtual bool IsMovable() const = 0;
//	// @brief ���� �ֺ��� �� �� appear ������ ����
//	virtual bool IsSendAppear() const = 0;
//	// @brief ��� ���� ����
	virtual bool IsDead() const = 0;

	virtual bool CanMove() const = 0;
	virtual bool CanResurrecton() const = 0;
	virtual bool HasTarget() const = 0;
	virtual bool HasNextRoutePosition() const = 0;

public: // get set
	virtual ActorUid_t GetActorUid() const = 0;
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

	// @brief ���� ��ġ(ĳ���� - ��Ȱ	��ġ, ���� - ���� ��ġ)
	virtual const PositionT& SpawnPosition() const = 0;
	virtual void SetSpawnPosition(const PositionT& _pos) = 0;

	virtual SkillIndex_t SkillIndex() const = 0;

	// @brief ���� �������κ��� �ִ� ������  �� �ִ� �Ÿ�
	virtual Distance_t MaxAroundDistance() const = 0;
	// @brief ���� ������ �Ÿ�
	// @details ���� ��ų�� �ִٸ� ��ų�� �����Ÿ�, ���ٸ� ������ �⺻�Ÿ�
	virtual Distance_t AttackRange() const = 0;

	virtual Angle_t Angle() const = 0;
	virtual void SetAngle(Angle_t _angle) = 0;

	virtual std::shared_ptr<Actor> Owner() const = 0;
	virtual std::shared_ptr<User> OwnerUser() const = 0;
};