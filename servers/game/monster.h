#pragma once

#include "actor.h"

class Monster : public Actor
{
public:
	Monster();
	virtual ~Monster();

	// @brief DataTable에서 기본 스탯/진영/종족을 로드하고 Actor를 초기화
	bool Initialize(MonsterIdx monster_idx, Level level);

public: // virtual
	void OnUpdate(float delta_time) override;
	void ProcessMessages(std::unique_ptr<Message> message) override;
	void OnDeath() override;
	makga::AIType GetAIType() const override;
	makga::ActorType GetActorType() const override;

	Attack  GetAtk() const override { return atk_; }
	Defense GetDef() const override { return def_; }

public:
	makga::MonsterFaction GetFaction() const { return faction_id_; }
	void SetFaction(makga::MonsterFaction faction) { faction_id_ = faction; }

	makga::RaceType GetRaceType() const { return race_type_; }
	void SetRaceType(makga::RaceType race) { race_type_ = race; }

	MonsterIdx GetMonsterIdx() const { return monster_idx_; }
	Level GetLevel() const { return level_; }

	// @brief 진영 어그로 전파 — 같은 진영 몬스터에게 MonsterAggravateMessage 전송
	void BroadcastAggravate(ActorId target_id);

private:
	// @brief 진영 기반 적대 판단
	bool IsHostileTo(const Actor& other) const override;

	MonsterIdx monster_idx_     = 0;
	Level      level_           = 1;
	Attack     atk_             = 0;
	Defense    def_             = 0;
	ActorId    last_attacker_id_ = 0;  // 마지막 피격자 ID (드랍 귀속에 사용)

	makga::AIType         ai_type_    = makga::AIType_NonAggressive;
	makga::MonsterFaction faction_id_ = makga::MonsterFaction_None;
	makga::RaceType       race_type_  = makga::RaceType_Human;
};
