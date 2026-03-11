---
applyTo: "**/*.{cpp,h,ixx}"
---

# C++ Conventions

## Naming
| Element | Convention | Example |
|---------|-----------|---------|
| Class/Struct | PascalCase | `ComponentManager` |
| Public method | PascalCase | `Initialize()` |
| Private method | snake_case | `validate_move()` |
| Member | snake_case_ | `next_id_` |
| Local | snake_case | `target_pos` |
| Enum type/value | PascalCase | `ActorType::Player` |
| Type alias | PascalCase | `using ActorId = uint32_t;` |
| Macro/constant | UPPER_SNAKE_CASE | `MAX_PACKET_SIZE` |
| Module/namespace | lowercase::lowercase | `makga::math` |

## Formatting
- Tab indent, Allman braces
- Constant-left: `if (nullptr == ptr)`, `if (0 == count)`
- Smart pointers: `make_shared` / `make_unique`

## Include Order
1. Project headers (`#include "actor.h"`)
2. Third-party headers
3. C++20 imports (`import makga.math.vector3;`)
4. Standard library (`#include <vector>`)

## C++20 Modules (`libs/cpp/libraries/include/*.ixx`)
- `export module <dotted.name>;` + `export namespace makga::xxx { ... }`
- Never `.h`/`.hpp` in `libs/cpp/`

## Patterns
- **ComponentManager**: `shared_mutex` (parallel reads, exclusive writes)
- **Singleton**: inherit `Singleton<T>`, use `T::Instance()`
- **Messaging**: `actor->PushMessage(msg)` only — no direct state mutation

## Comments
- `// @brief` for methods, `// ─── Section ───` for sections
- Korean OK for domain logic

## Unit Tests
- `*_test.cpp` in `unit_test/`, reference `pch.h`/`pch.cpp`
