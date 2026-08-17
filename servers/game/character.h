#pragma once

#include "actor.h"
#include "actor_messages.h"
#include "../common/inventory.h"
#include "../common/equip_slot.h"
#include "../common/skill_slot.h"
#include "../common/character_status.h"

// @brief 제작 숙련도 (직종별)
struct CraftProficiency
{
int32_t level = 1;
int64_t exp   = 0;
};

class Character : public Actor
{
public:
Character();
virtual ~Character();

public: // virtual
void OnUpdate(float delta_time) override;
void ProcessMessages(std::unique_ptr<Message> message) override;
makga::AIType GetAIType()   const override;
makga::ActorType GetActorType() const override;

public: // ─── 패킷 전송 ─────────────────────────────────────────────────────
// @brief User가 enter_world 시 패킷 전송 함수를 바인딩한다.
void SetSendFn(std::function<void(std::shared_ptr<NetPacket>)> fn);

// @brief Character → Client 방향 패킷 전송
void SendPacket(std::shared_ptr<NetPacket> packet) const;

// @brief 모든 장착 장비의 스탯을 해제 후 재적용 (강화 등으로 스탯 변경 시)
void ReapplyAllEquipStats();

public: // ─── 캐릭터 이름 ──────────────────────────────────────────────────────
std::string GetName() const { return name_; }
void        SetName(const std::string& name) { name_ = name; }

public: // ─── 인벤토리 ─────────────────────────────────────────────────────
Inventory&  GetInventory() { return inventory_; }
EquipSlot&  GetEquipSlot() { return equip_slot_; }

public: // ─── 스킬 ────────────────────────────────────────────────────────
SkillSlot&       GetSkillSlot()       { return skill_slot_; }
const SkillSlot& GetSkillSlot() const { return skill_slot_; }

public: // ─── 버프/상태이상 ────────────────────────────────────────────────
CharacterStatus&       GetStatus()       { return status_; }
const CharacterStatus& GetStatus() const { return status_; }

public: // ─── 소셜 ─────────────────────────────────────────────────────────
uint64_t  GetPartyId()  const { return party_id_; }
void      SetPartyId(uint64_t id)  { party_id_ = id; }

uint64_t  GetGuildId()  const { return guild_id_; }
void      SetGuildId(uint64_t id)  { guild_id_ = id; }

UserUid   GetUserUid()  const { return user_uid_; }
void      SetUserUid(UserUid uid)  { user_uid_ = uid; }

public: // ─── 스탯 / 레벨 ──────────────────────────────────────────────────
Level GetLevel() const { return level_; }
void  SetLevel(Level lv) { level_ = lv; }

Exp   GetExp()   const { return exp_; }
void  AddExp(Exp amount);

int64_t GetGold()     const { return gold_; }
void    AddGold(int64_t amount);
bool    ConsumeGold(int64_t amount);

int64_t GetRuby()     const { return ruby_; }
void    AddRuby(int64_t amount);

makga::JobType GetJob() const { return job_type_; }
void           SetJob(makga::JobType job) { job_type_ = job; }

makga::RaceType GetRace() const { return race_type_; }
void            SetRace(makga::RaceType race) { race_type_ = race; }

public: // ─── 제작 숙련도 ──────────────────────────────────────────────────
CraftProficiency& GetCraftProficiency(makga::CraftType type)
{
return craft_levels_[static_cast<int32_t>(type)];
}
int32_t GetCraftLevel(makga::CraftType type) const
{
auto it = craft_levels_.find(static_cast<int32_t>(type));
if (craft_levels_.end() == it) return 1;
return it->second.level;
}

public: // ─── 던전 입장 카운트 ─────────────────────────────────────────────
// key = dungeon_idx, value = 오늘 입장 횟수 (서버 DB와 매 입장시 동기화)
std::map<int64_t, int32_t>& GetDungeonEntryMap() { return dungeon_daily_entry_; }

public: // ─── 칭호 ─────────────────────────────────────────────────────────
int64_t  GetEquippedTitle() const { return equipped_title_idx_; }
void     SetEquippedTitle(int64_t idx) { equipped_title_idx_ = idx; }
uint64_t GetEquippedTitleStatKey() const { return title_stat_key_; }
void     SetEquippedTitleStatKey(uint64_t key) { title_stat_key_ = key; }

private:
void HandleAttackMessage(AttackMessage* msg);
void HandleReceiveLoot(ReceiveLootMessage* msg);
void HandleSkillUse(SkillUseMessage* msg);
void HandleApplyBuff(ApplyBuffMessage* msg);
void HandleExpGain(ExpGainMessage* msg);
void HandleEquipChange(EquipChangeMessage* msg);

// @brief 레벨 테이블 기준 레벨업 처리
void ProcessLevelUp();

// @brief 장비 스탯 효과 적용/해제
void ApplyEquipStats(makga::EquipSlot slot, bool apply);

// @brief 세트 효과 체크 및 적용
void UpdateSetEffects();

private: // ─── 인벤토리 & 장비 ──────────────────────────────────────────────
Inventory       inventory_;
EquipSlot       equip_slot_;

private: // ─── 스킬 & 버프 ───────────────────────────────────────────────────
SkillSlot       skill_slot_;
CharacterStatus status_;

private: // ─── 스탯 / 진행 ───────────────────────────────────────────────────
Level           level_      = 1;
Exp             exp_        = 0;
int64_t         gold_       = 0;
int64_t         ruby_       = 0;
makga::JobType  job_type_   = makga::JobType_None;
makga::RaceType race_type_  = makga::RaceType_Human;

private: // ─── 소셜 ─────────────────────────────────────────────────────────
UserUid         user_uid_   = 0;
uint64_t        party_id_   = 0;
uint64_t        guild_id_   = 0;

private: // ─── 칭호 ─────────────────────────────────────────────────────────
int64_t         equipped_title_idx_     = 0;
uint64_t        title_stat_key_         = 0;    // Stat::Key for equipped title bonus

private: // ─── 제작 ────────────────────────────────────────────────────────
std::map<int32_t, CraftProficiency> craft_levels_;      // CraftType → CraftProficiency

private: // ─── 던전 ─────────────────────────────────────────────────────────
std::map<int64_t, int32_t> dungeon_daily_entry_;        // dungeon_idx → 오늘 입장 횟수

private: // ─── 장비 스탯 키 추적 ───────────────────────────────────────────────
// slot → stat_->AddStat() 반환 키 목록 (해제 시 DelStat 사용)
std::map<makga::EquipSlot, std::vector<uint64_t>> equip_stat_keys_;

private: // ─── 패킷 전송 ────────────────────────────────────────────────────
std::function<void(std::shared_ptr<NetPacket>)> send_fn_;

private: // ─── 이름 ─────────────────────────────────────────────────────────
std::string name_;
};