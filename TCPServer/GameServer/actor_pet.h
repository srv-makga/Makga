#pragma once

#include "actor.h"

class Pet// : public Actor
{
public:
	Pet();
	virtual ~Pet();

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
//	// @breif ���濡�� ���� ����� �Լ�
//	void SendDelete(std::shared_ptr<Actor> _to_actor, eActorMoveEffect _effect) override;
//	void SendDelete(const std::vector<std::shared_ptr<Actor>>& _to_actors, eActorMoveEffect _effect) override;
//	void SendDelete(const std::unordered_set<std::shared_ptr<Actor>>& _to_actors, eActorMoveEffect _effect) override;
//
//	// @brief �� ã��
//	std::shared_ptr<Actor> FindTarget() override;
//	bool HasTarget() override;
//	std::shared_ptr<Actor> Target() const override;
//
//	// @brief �ش� ���͸� ������ �� �ִ� ��������
//	bool CanSeeActor(std::shared_ptr<Actor> _actor) override;
//
//	void SetRouteTable(TableIdx_t _idx) override;
//
//	// @brief Ư�� ��ǥ ����Ʈ�� �ݺ������� �̵��ϱ� ���� ����
//	void SetRoute(Sequence_t _seq, const Vector_t& _vec) override;
//
//	// @brief Ư�� ������� ����
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
//	flatbuffers::Offset<fb::ActorAppear> OffsetActorAppear(FB_BUILDER& _fbb, eActorMoveEffect _effect);
//	flatbuffers::Offset<fb::ActorDisAppear> OffsetActorDisappear(FB_BUILDER& _fbb, eActorMoveEffect _effect);
//	flatbuffers::Offset<fb::ActorMove> OffsetActorMove(FB_BUILDER& _fbb);
//	flatbuffers::Offset<fb::ActorInfoBase> OffsetActorInfoBase(FB_BUILDER& _fbb) override;
//	flatbuffers::Offset<fb::ActorInfoDetail> OffsetActorInfoDetail(FB_BUILDER& _fbb) override;
//#pragma endregion
//
//protected:
//	// @brief �̵��� ��ǥ ���� ����
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
//	// @brief ������ �� �ִ�
//	bool IsAttackable() const;
//	// @brief ���ݹ� �� �ִ�
//	bool IsAttacked() const;
//	// @brief �̵��� ������
//	bool IsMovable() const override;
//	// @brief ���� �ֺ��� �� �� appear ������ ����
//	bool IsSendAppear() const override;
//	// @brief ��� ���� ����
//	bool IsDead() const override;
//
//	SkillIndex_t SkillIndex() const override;
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
//public:
//	std::shared_ptr<ActorAI> AI() const override;
//
//private:
//	ActorUid_t m_uid;
//
//	std::string m_name;
//	fb::PositionT m_pos;
//	fb::HpMpT m_cur_hp_mp;
//	fb::HpMpT m_max_hp_mp;
//
//	Speed_t m_speed;
//
//	Terrain* m_terrain;
//	TerrainGrid* m_terrain_grid;
//
//	std::shared_ptr<Actor> m_target;
//
//	std::shared_ptr<Actor> m_owner;
//
//	std::shared_ptr<ActorAI> m_ai;
};