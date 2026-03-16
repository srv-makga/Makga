#pragma once

#include "../common/message_actor.h"

import makga.lib.pattern.singleton;
import makga.lib.lock;

class Actor;

// Actor 중앙 관리 및 메시지 라우팅
class ActorManager : public makga::lib::Singleton<ActorManager>
{
public:
	ActorManager() = default;
	virtual ~ActorManager() = default;

	// Actor 등록/해제
	void RegisterActor(ActorId actor_id, std::weak_ptr<Actor> actor);
	void UnregisterActor(ActorId actor_id);

	// 단일 Actor에게 메시지 전송
	bool SendMessageToActor(ActorId target_id, std::unique_ptr<Message> message);

	// 여러 Actor에게 Broadcast (메시지 1개만 생성, 여러 큐에 복사)
	void BroadcastToActors(const std::vector<ActorId>& actor_ids, 
	                       std::unique_ptr<Message> message);

	// Actor 조회
	std::shared_ptr<Actor> GetActor(ActorId actor_id);

private:
	// ActorId -> Actor weak_ptr 맵핑 (thread-safe)
	makga::lib::SharedMutex actors_mutex_;
	std::unordered_map<ActorId, std::weak_ptr<Actor>> actors_;
};
