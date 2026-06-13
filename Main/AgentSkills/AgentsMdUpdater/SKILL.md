---
name: agents-md-updater
description: "Use this skill when the user wants to maintain AGENTS.md files: capture evidence-based session learnings, compact project knowledge, update agent notes, or review existing guidance for staleness."
---

# AGENTS.md Updater

Act as a fast, professional note taker and summarizer. Preserve durable project knowledge that will make future investigation faster.

## Workflow

### 1. Gather Evidence

- Review the full session, including corrections and rejected designs, then inspect supporting code, tests, docs, logs, and git state (`status`, `diff`, history/blame when useful).
- Record only evidence-backed conclusions. Generalize from the observed case before writing guide text.

### 2. Choose Scope

- Put each learning in the closest appropriate `AGENTS.md`; use nested guides for progressive disclosure.
- Keep parent guides broad. Move symbol/path/command/format/workflow details into a deeper existing guide or a new scoped guide when they would make the parent implementation-heavy.
- Create scoped guides for clusters of durable local rules, not one-off facts.

### 3. Write Compact Notes

- Write concise bullets with durable boundaries, invariants, ownership rules, or preferred patterns.
- Omit implementation narration, code mechanics, concrete examples, pending fixes, local workarounds, transient status, speculation, and details better kept in source comments, tests, plans, or issues.
- Preserve existing notes unless evidence shows they are stale; update corrected boundaries such as deprecated areas.

### 4. Audit Existing Notes

For each `AGENTS.md` file modified:

- Randomly select 3 existing notes outside the new material, or all notes if fewer exist.
- Verify them against current code, documentation, or logs; update or remove stale notes and consolidate duplication across parent and child scopes.

### 5. Verify

- Review the final diff for scope, hierarchy, concision, and duplicated guidance, then run `git diff --check` on edited `AGENTS.md` files.
- Report updated and newly created files, evidence for new notes, and the 3-note stale audit results.
