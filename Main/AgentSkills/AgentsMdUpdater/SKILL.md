---
name: agents-md-updater
description: "Maintain AGENTS.md files with concise, durable project guidance. Use when Codex needs to capture general session learnings, update scoped agent notes, remove stale guidance, or audit AGENTS.md for usefulness. Do not use for changelogs, implementation summaries, or one-off facts that will not guide future work."
---

# AGENTS.md Updater

Act as a fast, professional editor. Preserve only durable project guidance that makes future implementation, review, or investigation smoother and more accurate without carrying session history forward.

## Overview

AGENTS.md files are operational context for coding agents, not human project documentation. Keep them thin, scoped, verified, and action-oriented.

## Workflow

### 1. Gather Evidence

- Review the full session, including corrections and rejected designs, then inspect supporting code, tests, docs, logs, and git state (`status`, `diff`, history/blame when useful).
- Treat existing AGENTS.md content as untrusted until checked; verify paths, commands, symbols, config names, and ownership boundaries against current source or docs before preserving or adding them.
- Verify command definitions by default. Run only allowed hygiene checks such as `git diff --check`, or commands the active repo rules and user request explicitly permit.
- Record only evidence-backed conclusions that generalize beyond the current patch or commit.

### 2. Apply Durability Filter

Before writing, each candidate note must pass all checks:

- It removes likely future ambiguity or prevents a realistic implementation mistake.
- It guides a future action, design choice, review, or investigation.
- It is likely to remain useful after local code moves, refactors, or the immediate issue is fixed.
- It states a reusable boundary, invariant, workflow preference, or evidence standard rather than a fact snapshot.
- It is agent-facing operational guidance, not a human-oriented overview, release note, or onboarding summary.
- It is not better kept in source comments, tests, commit messages, issue trackers, profiling ledgers, or final response summaries.

Reject notes that merely say what changed, which symbol currently does something, which patch worked, which file was touched, or what the latest code looks like unless that fact is the stable rule future agents must follow. Prefer a pointer to existing docs, tests, scripts, or source over duplicating their content.

### 3. Choose Scope

- Put each learning in the closest appropriate `AGENTS.md`; use nested guides for progressive disclosure.
- Keep parent guides broad. Use paths, symbols, commands, or formats mainly to define scope or an ownership boundary, not to preserve a session-specific implementation detail.
- Create scoped guides only for clusters of durable local rules; do not create a new guide to memorialize one patch, one TODO, or one narrow observation.
- Keep root guides thin: precedence, global invariants, and links to deeper scopes belong there; subsystem heuristics belong near the subsystem.

### 4. Write Compact Notes

- Write concise bullets with durable boundaries, invariants, ownership rules, preferred patterns, or evidence standards.
- Merge closely related facts into one note; avoid separate bullets that share the same owner, path, workflow, or invariant.
- Remove or consolidate duplicated guidance across parent/child guides and within the same guide before adding new material.
- Prefer one dense, readable sentence over multiple bullets when the facts are inseparable.
- Omit implementation narration, code mechanics, concrete examples, pending fixes, local workarounds, transient status, speculation, measurement logs, investigation results, and details better kept in source comments, tests, plans, or issues.
- Preserve existing notes unless evidence shows they are stale; update corrected boundaries such as deprecated areas.

### 5. Audit Existing Notes

For each `AGENTS.md` file modified:

- Randomly select 3 existing notes outside the new material, or all notes if fewer exist.
- Verify usefulness as well as truth: remove or generalize notes that are accurate but only describe past/current implementation details without guiding future work.
- Update or remove stale notes and consolidate duplication across parent and child scopes.

### 6. Independent Review

For broad, root-scope, or otherwise nontrivial AGENTS.md updates:

- Ask an independent agent to review the final diff against this skill's durability filter, concision requirements, and "no past burden" standard.
- Pass the review agent the raw diff and criteria, not a defense of the intended update.
- If the reviewer finds notes that are too narrow, too current-code-specific, too historical, duplicated, or not useful for future implementation accuracy, revise and request another review.
- For trivial wording or formatting changes, self-review against the same criteria.
- If independent agents are unavailable, do not imply that review happened; report the skip and apply the durability filter more strictly yourself.

### 7. Verify

- Review the final diff for scope, hierarchy, concision, duplicated guidance, and mergeable adjacent bullets; compact again before finishing if any edited section reads like a changelog, attempt log, or source-code tour.
- Confirm new paths and commands match the repo exactly. When command execution is disallowed, verify definitions instead of silently trusting names.
- Run `git diff --check` on edited `AGENTS.md` files.
- Report updated and newly created files, the durable rule captured, the stale/usefulness audit result, and the independent review result.

## Rejection Checks

- The diff reads like a changelog, investigation log, source tour, or list of recent commits.
- A new scoped AGENTS.md contains only one narrow observation.
- The same rule appears in parent and child scopes without a strong independent-trigger reason.
- Notes mention specific attempt outcomes, timings, TODO fixes, or symbol mechanics that source comments/tests should own.
- A path, command, macro, or ownership claim was copied without exact verification.
- A note is justified only by "this mattered during the session" or "this is what the code currently looks like."
