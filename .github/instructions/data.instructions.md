---
applyTo: "**/*.{sql,fbs}"
---

# Data Conventions

## SQL
- Tables: `snake_case` plural — `admin_accounts`; Columns: `snake_case`
- PK: `id` (auto-increment INT); FK: `<singular>_id`
- Boolean: `is_<state>`; Timestamps: `created_at`, `updated_at`

## FlatBuffers
- Table/enum: PascalCase; fields: snake_case; enum values: PascalCase
- Schemas: `enum.fbs`, `result.fbs`, `version.fbs`, `game_packet.fbs`, `game_packet_id.fbs`
- Compile: `scripts\flatbuffer\compile.bat` → `scripts\flatbuffer\out\cpp\`

## Packet Header (7 bytes)
`[packet_id: uint16 LE][packet_size: uint32 LE][key: uint8] + payload`
`NetPacket` inherits `makga::lib::Buffer<char>`; fields via `Header` struct (Id/Size/Key).
