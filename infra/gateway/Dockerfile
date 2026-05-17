# ─── Stage 1: 빌드 ───────────────────────────────────────────────────────────
FROM golang:1.22-alpine AS builder

# 빌드 의존성 설치
RUN apk add --no-cache git ca-certificates tzdata

WORKDIR /build

# 의존성 캐시 레이어 분리 (go.mod/go.sum만 먼저 복사)
COPY go.mod go.sum ./
RUN go mod download && go mod verify

# 소스 복사 및 빌드
COPY . .

# CGO 비활성화로 정적 바이너리 생성 (Alpine 호환)
# -ldflags "-s -w": 디버그 심볼 제거로 바이너리 크기 최소화
# -trimpath: 빌드 경로 정보 제거 (보안)
RUN CGO_ENABLED=0 GOOS=linux GOARCH=amd64 \
    go build \
    -ldflags="-s -w -X main.Version=$(git describe --tags --always --dirty 2>/dev/null || echo 'dev')" \
    -trimpath \
    -o /build/gateway \
    ./main.go

# ─── Stage 2: 실행 이미지 ────────────────────────────────────────────────────
# distroless: 쉘/패키지 관리자 없는 최소 이미지 (보안 강화)
FROM gcr.io/distroless/static-debian12:nonroot

# 타임존 데이터 복사 (로그 타임스탬프용)
COPY --from=builder /usr/share/zoneinfo /usr/share/zoneinfo
# CA 인증서 복사 (업스트림 HTTPS 호출용)
COPY --from=builder /etc/ssl/certs/ca-certificates.crt /etc/ssl/certs/

# 바이너리 복사
COPY --from=builder /build/gateway /gateway

# 설정 파일 복사 (기본값; 실제 배포 시 ConfigMap/볼륨으로 오버라이드)
COPY config/gateway.yaml /config/gateway.yaml

# nonroot 사용자로 실행 (보안)
USER nonroot:nonroot

# 포트 노출
EXPOSE 8080

# 헬스체크 (Docker 자체 헬스체크)
HEALTHCHECK --interval=10s --timeout=3s --start-period=5s --retries=3 \
    CMD ["/gateway", "-healthcheck"]

ENTRYPOINT ["/gateway"]
CMD ["-config", "/config/gateway.yaml"]
