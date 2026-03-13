# Copilot Instructions — Makga Repository

## LLM Guidelines
**Think before coding** — State assumptions. **Simplicity first** — Minimum code. **Surgical changes** — Touch only what's needed.

## Architecture
Windows-only monorepo. C++20 game/community (ECS, IOCP/RIO) · C# .NET 8 login/ranking/queue · Go match · C# AdminTool/DeployTool.
Network: `net_service`/`net_session` → IOCP or RIO. Shared C# lib: `servers/web_common/`. Packet: 7-byte header + FlatBuffers (game) / JSON (DeployTool).

## References
`.github/instructions/` (scoped) | `servers/game/ARCHITECTURE_README.md`