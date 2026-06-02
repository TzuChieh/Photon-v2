---
name: agents-md-updater
description: Use this skill when the user wants to maintain AGENTS.md files: capture evidence-based session learnings, compact project knowledge, update agent notes, or review existing guidance for staleness.
---

# AGENTS.md Updater

Act as a fast, professional note taker and summarizer. Preserve durable project knowledge that will make future investigation faster.

## Workflow

### 1. Gather Evidence

- Review the current session and inspect supporting code, logs, tests, and documentation before writing notes.
- Use `git status`, `git diff`, recent history, and `git blame` as useful clues. Do not assume uncommitted diffs exist.
- Record durable conclusions supported by evidence. Generalize from the observed case and keep useful lookup references such as paths, symbols, and commands.

### 2. Choose Scope

- Put each learning in the closest appropriate `AGENTS.md`. Create a scoped file when no suitable one exists, without excessive fragmentation.
- Keep parent guides at their own abstraction level. Compactly summarize relevant child areas so agents know where to dig deeper; do not list deeply nested paths from high-level guides.

### 3. Write Compact Notes

- Be concise and direct. Write agent- and human-readable bullets.
- Record only high-leverage rules that change how future work should be done.
- Omit implementation narration, examples, pending-task lists, transient status, speculation, and details that are easy to recover from nearby code.
- Preserve existing notes unless evidence shows they are stale.

### 4. Audit Existing Notes

For each `AGENTS.md` file modified:

- Randomly select 3 existing notes outside the new material, or all notes if fewer exist.
- Verify them against current code, documentation, or logs; update or remove stale notes.

### 5. Verify

- Review the final diff for scope, hierarchy, concision, and duplicated guidance, then run `git diff --check` on edited `AGENTS.md` files.
- Report updated and newly created files, evidence for new notes, and the 3-note stale audit results.
