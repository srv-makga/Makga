#pragma once

#include "sim_actor.h"

#include <vector>
#include <functional>

// ── NPC 일과표 항목 ───────────────────────────────────────────────────────────
struct NpcScheduleSlot
{
	uint8_t              hour;      // 0~23시
	NpcState             action;    // 이 시간에 수행할 FSM 상태
	makga::math::Vector3 target;    // 이동 목적지 (Schedule 상태 시)
};

// ── NPC Actor ────────────────────────────────────────────────────────────────
// @brief 순찰·일과표·감정 FSM을 갖는 NPC
class NpcAiController;

class NpcActor final : public SimActor
{
public:
	explicit NpcActor(ActorId id, uint32_t npc_id, NpcType npc_type);
	~NpcActor() override = default;

	bool Initialize() override;
	void Finalize()   override;
	void OnUpdate(float delta_time) override;

	makga::ActorType GetActorType() const override;

	// ── NPC 식별 ─────────────────────────────────────────────────────────────
	uint32_t GetNpcId()   const { return npc_id_; }
	NpcType  GetNpcType() const { return npc_type_; }

	// ── 상태 접근자 ───────────────────────────────────────────────────────────
	NpcState   GetState()   const { return state_; }
	NpcEmotion GetEmotion() const { return emotion_; }

	// ── 상태 변경 (AiController에서 호출) ────────────────────────────────────
	void TransitionTo(NpcState new_state);
	void SetEmotion(NpcEmotion emotion);

	// ── 일과표 설정 ───────────────────────────────────────────────────────────
	void AddScheduleSlot(NpcScheduleSlot slot);
	std::optional<NpcScheduleSlot> GetCurrentScheduleSlot(uint8_t current_hour) const;

	// ── 순찰 경로 설정 ────────────────────────────────────────────────────────
	void AddWaypoint(const makga::math::Vector3& wp);
	const std::vector<makga::math::Vector3>& GetWaypoints() const { return waypoints_; }
	int32_t GetCurrentWaypointIndex() const { return current_waypoint_; }
	void    AdvanceWaypoint();

	// ── 콘텐츠 ID (대화/상점/퀘스트) ─────────────────────────────────────────
	uint32_t GetContentId() const { return content_id_; }
	void     SetContentId(uint32_t id) { content_id_ = id; }

	// ── 상호작용 콜백 등록 ────────────────────────────────────────────────────
	using InteractCallback = std::function<void(ActorId player_actor_id, uint32_t npc_id, InteractType, uint32_t content_id)>;
	void SetInteractCallback(InteractCallback cb) { interact_cb_ = std::move(cb); }
	void InvokeInteract(ActorId player_actor_id, InteractType type);

private:
	uint32_t  npc_id_;
	NpcType   npc_type_;
	NpcState  state_{ NpcState::Idle };
	NpcEmotion emotion_{ NpcEmotion::Neutral };
	uint32_t  content_id_{ 0 };

	std::vector<NpcScheduleSlot>      schedule_;
	std::vector<makga::math::Vector3> waypoints_;
	int32_t                           current_waypoint_{ 0 };

	std::unique_ptr<NpcAiController>  ai_controller_;
	InteractCallback                  interact_cb_;
};
