#pragma once

#include "user_dispatcher.h"
#include "game_db_connector.h"

// ─── 로그인 상태 머신 ────────────────────────────────────────────────────────
// Connecting → (C2S_Login) → Authenticating → (DB 검증 완료) → CharacterSelect
// CharacterSelect → (C2S_SelectCharacter) → InGame
enum class LoginState : uint8_t
{
	Connecting,      // TCP 연결만 수립, 인증 전
	Authenticating,  // SessionToken DB 검증 중 (비동기)
	CharacterSelect, // 인증 완료, 캐릭터 선택 대기
	InGame,          // 캐릭터 월드 입장 완료
};

class User : public std::enable_shared_from_this<User>
{
	friend class UserDispatcher;

public:
	User();
	virtual ~User();

	bool Initialize();
	void Finalize();

	// @brief 접속 시 DB에서 유저 + 캐릭터 목록을 로드하여 User를 초기화한다.
	//        callback은 DB worker thread에서 호출된다.
	void LoadFromDb(UserUid uid, std::function<void(bool success)> callback);

	// @brief 로그인 시 DB에서 로드한 캐릭터 목록 세팅
	void set_character_list(std::vector<CharacterUid> uids) { character_uids_ = std::move(uids); }
	const std::vector<CharacterUid>& get_character_list() const { return character_uids_; }

	// @brief 캐릭터를 월드에 입장시켜 active_actor_id_ 세팅
	bool enter_world(CharacterUid char_uid);
	void leave_world();

	ActorId get_active_actor_id() const { return active_actor_id_; }
	bool has_active_actor() const { return 0 != active_actor_id_; }

	UserDispatcher& GetDispatcher() { return dispatcher_; }

	LoginState get_login_state() const { return login_state_; }
	void set_login_state(LoginState state) { login_state_ = state; }

	// @brief GameSession이 연결 직후 패킷 전송 함수를 바인딩한다.
	void SetSendFn(std::function<void(std::shared_ptr<NetPacket>)> fn);

	// @brief User → Client 방향 패킷 전송
	void SendPacket(std::shared_ptr<NetPacket> packet);

protected:
	UserUid                   user_uid_        = 0;
	std::vector<CharacterUid> character_uids_;
	ActorId                   active_actor_id_ = 0;
	LoginState                login_state_     = LoginState::Connecting;
	UserDispatcher            dispatcher_;

	std::function<void(std::shared_ptr<NetPacket>)> send_fn_;
};
