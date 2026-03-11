---
applyTo: "**/*.cs"
---

# C# Conventions

## Naming
| Element | Convention | Example |
|---------|-----------|---------|
| Class/Interface/Enum | PascalCase | `AccountsController` |
| Public method/property | PascalCase | `GetTableAsync()` |
| Private field | _camelCase | `_db` |
| Local/parameter | camelCase | `userId` |
| Async method | PascalCase + Async | `QueryAsync<T>()` |
| Constant | PascalCase | `DefaultPageSize` |

## Formatting
- Tab indent, Allman braces
- Constant-left: `if (null == user)`, `if ("admin" == role)`
- File-scoped namespaces: `namespace AdminTool.Controllers;`
- Nullable: always `?` for nullable params/returns
- Auto-property: `public List<T> Items { get; set; } = new();`
- Attributes: stack on one line `[HttpPost, ValidateAntiForgeryToken]`

## Directives
`using` order: `System.*` → `Microsoft.*` → app namespaces

## Async
All DB/network: `async`/`await` — never `.Result`/`.Wait()`

## Comments
- `// ─── Section ───` dividers; Korean OK
