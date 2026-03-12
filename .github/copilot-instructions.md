# Copilot Instructions — Makga Repository

## LLM Guidelines
**Think before coding** — State assumptions. **Simplicity first** — Minimum code. **Surgical changes** — Touch only what's needed.

---

## Architecture

Windows-only monorepo. C++ game/community servers (ECS, IOCP/RIO), C# services (.NET 8), Go match server. All share the same 7-byte packet header.

| Server | Language | Path |
|--------|----------|------|
| Game | C++20 | `servers/game/` |
| Community | C++20 | `servers/community/` |
| Login | C# .NET 8 | `servers/login/` |
| Ranking | C# .NET 8 | `servers/ranking/` |
| Queue | C# .NET 8 | `servers/queue/` |
| Match | Go | `servers/match/` |
| AdminTool | C# ASP.NET 8 | `tools/AdminTool/` |
| DeployTool | C# Blazor+Agent | `tools/DeployTool/` |

**Shared C# lib:** `servers/web_common/CSharp.Common.csproj` — referenced by Login and Ranking.

---

Network: `net_service`/`net_session` abstract interface → IOCP or RIO implementation.

---

## Packet Wire Format

```
[packet_id: uint16 LE][payload_len: uint32 LE][key: uint8] | payload
◄──────────── 7 bytes header ────────────────►
```

Game payload: **FlatBuffers** · DeployTool payload: **JSON**

---

## References
`.github/instructions/` (scoped per-language rules) | `servers/game/ARCHITECTURE_README.md`