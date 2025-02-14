#pragma once

#include "actor.h"
#include "actor_ai.h"

class ActorAI;

class Monster// : public Actor
{
public:
	Monster();
	virtual ~Monster();

//public:
//	void Initialize() override;
//	void Finalize() override;
//
//	bool SetTable(const ActorBasicTable* _table) override;
//	void OnUpdate() override;
//
//	Result_t DoMove(const PositionT& _position) override;
//	Result_t Move(Coord_t _x, Coord_t _y, Coord_t _z) override;
//
//	void SendDelete(std::shared_ptr<Actor> _to_actor, eActorMoveEffect _effect) override;
//	void SendDelete(const std::vector<std::shared_ptr<Actor>>& _to_actors, eActorMoveEffect _effect) override;
//	void SendDelete(const std::unordered_set<std::shared_ptr<Actor>>& _to_actors, eActorMoveEffect _effect) override;
//
//	std::shared_ptr<Actor> FindTarget() override;
//	bool HasTarget() override;
//	std::shared_ptr<Actor> Target() const override;
//
//	bool CanSeeActor(std::shared_ptr<Actor> _actor) override;
//
//	void SetRouteTable(TableIdx_t _idx) override;
//	void SetRoute(Sequence_t _seq, const Vector_t& _vec) override;
//
//	Result_t Change(TableIdx_t _index) override;
//	Result_t ChangeRollback() override;
//
//	bool CanResurrecton() const override;
//	Result_t Resurrecton() const override;
//
//	Result_t DoAttack(std::shared_ptr<Actor> _target, SkillIdx_t _skill_idx) override;
//
//	void StartAI() override;
//
//	void AddAggroList(std::shared_ptr<Actor> _actor) override;
//	void AddAggroList(const ActorList& _actor) override;
//	void SelectTarget() override;
//
//	void MulticastHpMp() override;
//	void MulticastBuff() override;
//	void Multicast(fb::server::RecvPid, flatbuffers::FlatBufferBuilder& _builder) override;
//
//	flatbuffers::Offset<fb::ActorAppear> OffsetActorAppear(FB_BUILDER& _fbb, eActorMoveEffect _effect);
//	flatbuffers::Offset<fb::ActorDisAppear> OffsetActorDisappear(FB_BUILDER& _fbb, eActorMoveEffect _effect);
//	flatbuffers::Offset<fb::ActorMove> OffsetActorMove(FB_BUILDER& _fbb);
//	flatbuffers::Offset<fb::ActorInfoBase> OffsetActorInfoBase(FB_BUILDER& _fbb) override;
//	flatbuffers::Offset<fb::ActorInfoDetail> OffsetActorInfoDetail(FB_BUILDER& _fbb) override;
//
//protected:
//	Result_t SetMoveTarget(Vector_t _vec) override;
//
//public:
//	ActorUid_t ActorUid() const override;
//	void SetActorUid()(ActorUid_t _uid) override;
//	fb::eActorType Type() const override;
//	bool IsCharacter() const override;
//	bool IsMonster() const override;
//	bool IsNpc() const override;
//	bool IsGadget() const override;
//	bool IsPet() const override;
//
//	bool IsAttackable() const override;
//	bool IsAttacked() const override;
//	bool IsMovable() const override;
//	bool IsSendAppear() const override;
//	bool IsDead() const override;
//
//	SkillIdx_t SkillIndex() const override;
//
//	void SetDefaultPosition() override;
//	void SetPosition(const PositionT& _pos) override;
//	const PositionT& Position() const override;
//	const PositionT& LastRoutePosition() const override;
//	bool HasNextRoutePosition() const override;
//	const PositionT& NextRoutePosition() const override;
//
//	Coord_t X() const override;
//	Coord_t Y() const override;
//	Coord_t Z() const override;
//
//	Coord_t MaxAroundDistance() const override;
//	Coord_t AttackRange() const override;
//
//	Distance_t MySight() const override;
//
//	Speed_t MoveBonusSpeed() const override;
//	Speed_t MoveSpeed() const override;
//
//	fb::eAiType AIType() const override;
//
//	Hp_t MaxBonusHp() const override;
//	Mp_t MaxBonusMp() const override;
//
//	Hp_t MaxHp() const override;
//	Hp_t CurHp() const override;
//	Mp_t MaxMp() const override;
//	Mp_t CurMp() const override;
//
//	Terrain* CurTerrain() const override;
//	TerrainGrid* CurTerrainGrid() const override;
//	void SetTerrainGrid(TerrainGrid* _grid) override;
//
//	std::shared_ptr<Actor> Owner() const override;
//	std::shared_ptr<User> OwnerUser() const override;
//
//	std::shared_ptr<ActorAI> AI() const override;
//
//private:
//	const MonsterBasicTable* m_basic_table;
//
//	ActorUid_t m_uid;
//
//	fb::PositionT m_position;
//
//	fb::HpMpT m_hp_mp_max;
//	fb::HpMpT m_hp_mp_cur;
//
//	// clan
//	bool m_is_leader;
//
//	Time_t m_last_move_time;
//
	std::shared_ptr<ActorAI> m_ai;
//	std::shared_ptr<Actor> m_target;
};