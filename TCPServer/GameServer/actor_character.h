#pragma once

#include "actor.h"

class Character : public Actor
{
public:
	Character();
	~Character();

public:
	void Initialize() override;
	void Finalize() override;

	bool Setup(const fb::CharacterDetail* _detail);
	bool Setup(const fb::CharacterDetailT& _detail);

	void SetActorTable(const ActorBasicTable* _table) override;
	void OnUpdate() override;

public:
	Result_t DoMove(fb::PositionT& _position) override;
	Result_t DoAttack(SkillIndex_t _index, SkillLevel_t _level, const PositionT& _cast_pos, OUT ActorList& _target_list) override;
	Result_t UpdatePosition(const fb::PositionT& _position, fb::eActorMoveEffect _move_effect) override;
	Result_t UpdatePosition(TerrainIdx_t _terrain_idx, const PositionT& _position, fb::eActorMoveEffect _move_effect) override;

public:
	bool CanMove() const override;

public: // get set
	Terrain* GetTerrain() const override;

	TerrainGrid* GetTerrainGrid() const override;
	void SetTerrainGrid(TerrainGrid* _grid) override;

	const PositionT& Position() const override;
	void SetPosition(const PositionT& _pos) override;
//
//	// @breif 상대방에게 나를 지우는 함수
//	void SendDelete(std::shared_ptr<Actor> _to_actor, eActorMoveEffect _effect) override;
//	void SendDelete(const std::vector<std::shared_ptr<Actor>>& _to_actors, eActorMoveEffect _effect) override;
//	void SendDelete(const std::unordered_set<std::shared_ptr<Actor>>& _to_actors, eActorMoveEffect _effect) override;
//
//	// @brief 적 찾기
//	std::shared_ptr<Actor> FindTarget() override;
//	bool HasTarget() override;
//	std::shared_ptr<Actor> Target() const override;
//
//	// @brief 해당 액터를 인지할 수 있는 상태인지
//	bool CanSeeActor(std::shared_ptr<Actor> _actor) override;
//
//	void SetRouteTable(TableIdx_t _idx) override;
//
//	// @brief 특정 좌표 리스트를 반복적으로 이동하기 위한 설정
//	void SetRoute(Sequence_t _seq, const Vector_t& _vec) override;
//
//	// @brief 특정 모습으로 변경
//	Result_t Change(TableIdx_t _index) override;
//	Result_t ChangeRollback() override;
//
//	bool CanResurrecton() const override;
//	Result_t Resurrecton() const override;
//
//	Result_t DoAttack(std::shared_ptr<Actor> _target, SkillIndex_t _skill_idx) override;
//
//	void StartAI() override;
//
//	void AddAggroList(std::shared_ptr<Actor> _actor) override;
//	void AddAggroList(const ActorList& _actor) override;
//	void SelectTarget() override;
//
//#pragma region broadcast & multicast
//public:
//	void MulticastHpMp() override;
//	void MulticastBuff() override;
//	void Multicast(fb::server::RecvPid, flatbuffers::FlatBufferBuilder& _builder) override;
//#pragma endregion
//
//#pragma region flatbuffer_offset 
//public:
//	flatbuffers::Offset<fb::ActorAppear> OffsetActorAppear(FB_BUILDER& _fbb, eActorMoveEffect _effect) override;
//	flatbuffers::Offset<fb::ActorDisAppear> OffsetActorDisappear(FB_BUILDER& _fbb, eActorMoveEffect _effect) override;
//	flatbuffers::Offset<fb::ActorMove> OffsetActorMove(FB_BUILDER& _fbb) override;
//	flatbuffers::Offset<fb::ActorInfoBase> OffsetActorInfoBase(FB_BUILDER& _fbb) override;
//	flatbuffers::Offset<fb::ActorInfoDetail> OffsetActorInfoDetail(FB_BUILDER& _fbb) override;
//#pragma endregion
//
//protected:
//	// @brief 이동할 목표 지점 설정
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
//	// @brief 공격할 수 있는
//	bool IsAttackable() const;
//	// @brief 공격받 수 있는
//	bool IsAttacked() const;
//	// @brief 이동이 가능한
//	bool IsMovable() const override;
//	// @brief 누가 주변에 올 때 appear 정보를 줄지
//	bool IsSendAppear() const override;
//	// @brief 사망 상태 여부
//	bool IsDead() const override;
//
//	SkillIndex_t SkillIndex() const override;
//
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
//public: // actor 자체
	//std::shared_ptr<ActorAI> AI() const override;

private:
	ActorUid_t m_uid;

	fb::PositionT  m_pos;
	fb::HpMpT m_cur_hp_mp;
	fb::HpMpT m_max_hp_mp;

	std::string m_name;
	Speed_t m_speed;

	Tick_t m_tick_prev_move;

	Terrain* m_terrain;
	TerrainGrid* m_grid;

	std::shared_ptr<ActorAI> m_ai;

	std::shared_ptr<User> m_user;
};