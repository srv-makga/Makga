#pragma once

#include "../common/message_actor.h"

import makga.lib.pattern.singleton;
import makga.lib.lock;

class Actor;
class ActorRef;
class Props;

// @brief Actor 중앙 레지스트리 및 메시지 라우팅
class ActorSystem : public makga::lib::Singleton<ActorSystem>
{
public:
	ActorSystem() = default;
	virtual ~ActorSystem() = default;

	// Actor 등록/해제
	void Register(ActorId actor_id, std::weak_ptr<Actor> actor);
	void Unregister(ActorId actor_id);

	// @brief 단일 Actor에게 메시지 전송 (MPSC 큐 push, 비동기)
	bool Tell(ActorId target_id, std::unique_ptr<Message> message);

	// @brief 여러 Actor에게 브로드캐스트 (Clone 사용, 마지막 대상에 move)
	void Broadcast(const std::vector<ActorId>& actor_ids, std::unique_ptr<Message> message);

	// @brief Actor 조회 (같은 스레드에서만 상태 접근 허용)
	std::shared_ptr<Actor> GetActor(ActorId actor_id);

	// @brief ActorRef 생성
	ActorRef GetRef(ActorId actor_id);

private:
	makga::lib::SharedMutex actors_mutex_;
	std::unordered_map<ActorId, std::weak_ptr<Actor>> actors_;
};
