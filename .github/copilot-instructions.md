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

## Build Commands (root: C:\Github\Makga)

```powershell
msbuild .\servers\servers.sln /p:Configuration=Debug /p:Platform=x64
msbuild .\servers\servers.sln /p:Configuration=Release /p:Platform=x64
msbuild .\unit_test\unit_test.vcxproj /p:Configuration=Debug /p:Platform=x64
scripts\flatbuffer\compile.bat                          # run after any .fbs change

cd servers\login   && dotnet restore && dotnet run      # :6000
cd servers\ranking && dotnet restore && dotnet run
cd servers\queue   && dotnet restore && dotnet run
cd tools\AdminTool && dotnet restore && dotnet run      # :5000
cd tools\DeployTool\Agent   && dotnet restore && dotnet run   # :7700
cd tools\DeployTool\Manager && dotnet restore && dotnet run   # :5000
```

**Single test:** VS Test Explorer → `unit_test\unit_test.vcxproj`, filter by name (e.g. `mpsc_queue_test`).

---

## Game Server ECS (`servers/game/`)

```
GameWorld (Singleton)
├── ComponentManager  SoA: ComponentArray<T> = dense vector + sparse map, std::shared_mutex
│   └── Transform / Physics / Stat / AI / Combat / Terrain components
├── Systems (logic only)  Physics → Movement → Combat → AI  (per-frame order)
├── TickGroupManager  CPU-core-count groups, round-robin, each group = 1 thread
└── NewActor  data-only container, ActorId + MPSC ConcurrentQueue<ActorMessage>
```

**Legacy vs New (migration Phase 5–6 pending):** `Actor→Character/Monster/Gadget` = legacy. `NewActor` = new ECS. **Do not mix.** Prefer `NewActor` for new features.

Network: `net_service`/`net_session` abstract interface → IOCP or RIO implementation. Community: `Party`, `Guild`, `Friend` all inherit `Community` base.

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