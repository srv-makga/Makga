#pragma once

/*
*	시스템			대표 성격
1	Terrain			단일 스레드, 시뮬레이션
2	PartyService	글로벌 상태 + 메시지 큐
3	CombatSystem	계산 중심, 맵 종속
4	InventorySystem	트랜잭션 / 영속성
5	PacketRouter	IO → Logic 경계
*/

#include "user_server.h"
#include "net_thread.h"
#include "job_thread.h"
#include "world_subsystem.h"
#include "terrain_manager.h"
#include "../common/timer.h"

// New Component-based systems
#include "component_manager.h"
#include "movement_system.h"
#include "physics_system.h"
#include "ai_system.h"
#include "combat_system.h"
#include "tick_group_manager.h"

import makga.lib.pattern.singleton;
import makga.lib.pattern.objectpool;

class GameWorld final : public makga::lib::Singleton<GameWorld>
{
public:
	GameWorld() = default;
	~GameWorld() = default;

	bool Initialize();
	void Finalize();

	void Start();
	void Shutdown();

	void OnUpdate();

	template<typename T>
	T * GetSubsystem()
	{
		return static_cast<T*>(subsystems_[T::StaticTypeId()]);
	}

	// New component-based API
	ComponentManager& GetComponentManager() { return component_manager_; }
	TickGroupManager& GetTickGroupManager() { return tick_group_manager_; }
	
	template<typename T>
	T* GetWorldSystem()
	{
		for (auto* system : world_systems_)
		{
			if (auto* casted = dynamic_cast<T*>(system))
				return casted;
		}
		return nullptr;
	}

public:
	Tick last_update_tick_;

	// ----- Core -----
	std::thread thread_;

	// 네트워크 모듈
	std::shared_ptr<makga::network::IocpCore> core_;
	std::shared_ptr<UserServer> user_server_;

	// 스케줄러 스레드
	std::unique_ptr<Timer> timer_;

	std::vector<std::unique_ptr<WorldSubsystem>> subsystems_;
	std::array<std::vector<WorldSubsystem*>, makga::TickGroup_MAX> subsystems_tick_groups_;
	
	// ----- New Component-based Architecture -----
	ComponentManager component_manager_;
	TickGroupManager tick_group_manager_;
	
	std::vector<WorldSystem*> world_systems_;
	std::unique_ptr<MovementSystem> movement_system_;
	std::unique_ptr<PhysicsSystem> physics_system_;
	std::unique_ptr<AISystem> ai_system_;
	std::unique_ptr<CombatSystem> combat_system_;
	
	// ----- World Data -----
	// 맵 목록
	TerrainManager terrain_manager_;

	// ----- Global Systems -----
	// 플레이어 정보
	// 유저 정보
	// 
	// 파티 서비스
	// 당 서비스
	// 동맹 서비스

	// 채팅 서비스
	// 우편 서비스
	// 거래 서비스
	// 거래소 서비스

	// 이벤트 서비스
	// 랭킹 서비스

	// DB 서비스

	// 매트릭 서비스

	/*
	* 🧠 Core / Infrastructure (10)
	NetworkService (IOCP / epoll)
	SessionManager
	PacketDispatcher
	TimerService (wheel / hierarchical)
	LogicScheduler
	ThreadPool (IO 전용, 비동기 DB 전용)
	JobQueue / MessageQueue
	ConfigService (핫리로드)
	TimeService (서버 기준 시간)
	UIDGenerator (분산 ID)

	🌍 World / Map (10)
	Terrain (맵 단일 스레드)
	TerrainGrid (셀, 시야 단위)
	MapLoader
	SpawnManager
	ZoneTrigger (안전지대, 전장)
	TeleportService
	InstanceManager (던전)
	WorldEventManager (침공, 보스)
	PathFindingService
	VisibilityService

	👤 Player / Character (10)
	PlayerManager
	Character (영속 객체)
	Actor (기본 유닛)
	StatComponent
	LevelComponent
	ExpComponent
	PKComponent
	AlignmentComponent (카오/질서)
	TitleComponent
	AppearanceComponent

	⚔ 전투 시스템 (10)
	CombatSystem
	DamageCalculator
	SkillSystem
	SkillTree
	BuffDebuffSystem
	AggroSystem
	HitDetection
	CooldownManager
	StatusEffectProcessor
	DeathPenaltySystem

	🤖 Monster / AI (10)
	MonsterManager
	Monster
	AIController
	BehaviorTree
	Blackboard
	ThreatTable
	SpawnGroup
	BossPhaseController
	AIEventRouter
	LootTable

	🎒 아이템 / 경제 (10)
	ItemManager
	Inventory
	Equipment
	ItemInstance
	DropSystem
	TradeService
	AuctionService
	CurrencyService
	AntiDuplicationService
	LogEconomyService

	🛠 생활 / 제작 (10)
	GatheringSystem
	CraftingSystem
	RecipeManager
	SkillLifeComponent
	DurabilitySystem
	ToolSystem
	ProductionQueue
	FarmingSystem
	FishingSystem
	CookingSystem

	🏠 하우징 (5)
	HousingManager
	HouseInstance
	FurnitureSystem
	AccessPermissionSystem
	TaxMaintenanceSystem

	👥 소셜 (10)
	PartyService
	Party
	GuildService
	Guild
	AllianceService
	FriendService
	BlockService
	EmoteSystem
	VoiceChatHook
	SocialLogService

	💬 커뮤니케이션 (5)
	ChatService
	ChannelManager
	WhisperService
	BroadcastService
	SpamFilter

	💾 Persistence / Ops (10)
	DBService
	AsyncSaveJob
	SaveScheduler
	BackupService
	RollbackService
	GMService
	MetricsService
	AlertService
	CrashRecovery
	LivePatchService
	*/
};