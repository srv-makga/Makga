package proxy

// 컨텍스트 키 타입 (string 타입 충돌 방지)
type ctxKey string

const (
	// CtxKeyAccountUID는 인증된 계정 UID를 저장하는 컨텍스트 키다.
	CtxKeyAccountUID ctxKey = "account_uid"
	// CtxKeyRole은 인증된 계정 역할을 저장하는 컨텍스트 키다.
	CtxKeyRole ctxKey = "role"
	// CtxKeyRequestID는 요청 고유 ID를 저장하는 컨텍스트 키다.
	CtxKeyRequestID ctxKey = "request_id"
	// CtxKeyCluster는 라우팅된 클러스터 이름을 저장하는 컨텍스트 키다.
	CtxKeyCluster ctxKey = "cluster"
)
