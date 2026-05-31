---
name: doc-expert
description: Use this skill when the user wants to add, review, or update Markdown guides and C++ Doxygen documentation. For AGENTS.md project-memory maintenance, use update-agents-md instead.
---

# Documentation Expert

Document verified behavior, not assumptions.

## Workflow

### 1. Select Targets

- Read scoped `AGENTS.md`. Use `git status`, `git diff`, recent history, and `git blame` to find changed or stale documentation.
- Prioritize the requested surface, then public headers, guide entry points, and frequently used symbols. Use `rg` for references and check [documented-classes.md](references/documented-classes.md) for known C++ examples.

### 2. Verify and Write

- Read supporting code, call sites, and referenced workflows to verify claims, side effects, units, coordinate systems, ownership, and corner cases.
- Match nearby Markdown or Doxygen style. For C++, prefer concise `/*! @brief ... */` comments and add `@param` or `@return` details when they clarify the contract.
- For C++ docs, use [style-examples.md](references/style-examples.md) and the [coding standard](../../Documentation/coding_standard.md).

### 3. Check

- Edit with the available patch tool, review the diff, and run `git diff --check`.
- Ask the user when code intent remains ambiguous after inspection.
