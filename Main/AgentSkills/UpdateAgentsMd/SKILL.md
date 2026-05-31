---
name: update-agents-md
description: Use this skill when the user wants to maintain AGENTS.md files: capture evidence-based session learnings, compact project knowledge, update agent notes, or review existing guidance for staleness.
---

# Update AGENTS.md

Act as a fast, professional note taker and summarizer. Preserve durable project knowledge that will make future investigation faster.

## Workflow

### 1. Gather Evidence

- Review the current session and inspect supporting code, logs, tests, and documentation before writing notes.
- Use `git status`, `git diff`, recent history, and `git blame` as useful clues. Do not assume uncommitted diffs exist.
- Record durable conclusions supported by evidence. Generalize from the observed case and keep useful lookup references such as paths, symbols, and commands.

### 2. Choose Scope

- Put each learning in the closest appropriate `AGENTS.md`. Create a scoped file when no suitable one exists, without excessive fragmentation.
- Maintain a note hierarchy: parent `AGENTS.md` files should compactly summarize or index relevant child `AGENTS.md` guidance so readers can navigate from the repo root.

### 3. Write Compact Notes

- Be concise and direct. Write agent- and human-readable bullets.
- Capture non-obvious architecture, invariants, workflows, test locations, and efficient lookup paths with enough evidence references for quick verification.
- Omit transient status, one-off details, speculation, and redundant guidance. Preserve existing notes unless evidence shows they are stale.

### 4. Audit Existing Notes

For each `AGENTS.md` file modified:

- Randomly select 3 existing notes outside the new material, or all notes if fewer exist.
- Verify them against current code, documentation, or logs; update or remove stale notes.

### 5. Verify

- Review the final diff for scope, hierarchy, concision, and duplicated guidance, then run `git diff --check` on edited `AGENTS.md` files.
- Report updated and newly created files, evidence for new notes, and the 3-note stale audit results.
