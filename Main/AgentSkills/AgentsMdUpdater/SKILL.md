---
name: agents-md-updater
description: "Maintain AGENTS.md as compact execution context for coding agents. Use when Codex needs to capture durable learnings, update scoped notes, remove stale guidance, or audit AGENTS.md for usefulness. Do not use for changelogs, implementation summaries, or one-off facts."
---

# AGENTS.md Updater

Optimize AGENTS.md as execution context injected into coding agents. Preserve only durable
instructions that change how a future agent should implement, review, investigate, or verify work.

## Consumption Model

- AGENTS.md is agent input, not human project documentation, onboarding, or a session record. Every
  sentence spends context; keep only text that improves a future agent's decisions.
- Make the trigger or scope and the required action, invariant, ownership boundary, or evidence gate
  directly recoverable from each note.
- Keep orientation only when it routes an agent to the correct owner, source, command, or deeper
  guide; omit marketing, prose tours, and explanations that do not change action.

## Workflow

### 1. Gather Evidence

- Review the full session, including corrections and rejected designs, then inspect supporting code, tests, docs, logs, and git state (`status`, `diff`, history/blame when useful).
- Treat existing AGENTS.md content as untrusted until checked; verify paths, commands, symbols, config names, and ownership boundaries against current source or docs before preserving or adding them.
- Verify command definitions by default. Run only allowed hygiene checks such as `git diff --check`, or commands the active repo rules and user request explicitly permit.
- Record only evidence-backed conclusions that generalize beyond the current patch or commit.

### 2. Apply Durability Filter

Before writing, each candidate note must pass all checks:

- It removes likely future ambiguity or prevents a realistic implementation mistake.
- A future agent can identify when it applies and what action, constraint, or verification follows.
- It is likely to remain useful after local code moves, refactors, or the immediate issue is fixed.
- It states a reusable boundary, invariant, workflow preference, or evidence standard rather than a fact snapshot.
- It can be applied directly without interpreting motivation-heavy or human-oriented narrative.
- It is not better kept in source comments, tests, commit messages, issue trackers, profiling ledgers, or final response summaries.

Reject notes that merely say what changed, describe what a symbol currently does, tour the project,
or preserve a patch outcome unless that fact is the stable rule future agents must follow. Prefer a
retrieval pointer to authoritative docs, tests, scripts, or source over duplicating their content.

### 3. Choose Scope

- Put each learning in the closest appropriate `AGENTS.md`; use nested guides for progressive disclosure.
- Keep parent guides broad. Use paths, symbols, commands, or formats mainly to define scope or an ownership boundary, not to preserve a session-specific implementation detail.
- Create scoped guides only for clusters of durable local rules; do not create a new guide to memorialize one patch, one TODO, or one narrow observation.
- Keep root guides thin: precedence, global invariants, and links to deeper scopes belong there; subsystem heuristics belong near the subsystem.

### 4. Write Compact Notes

- Lead with the trigger, owner, action, or constraint. Use imperative wording; include rationale only
  when it prevents a wrong choice.
- Write concise bullets with durable boundaries, invariants, ownership rules, preferred patterns, or
  evidence standards.
- Merge closely related facts into one note; avoid separate bullets that share the same owner, path, workflow, or invariant.
- Remove or consolidate duplicated guidance across parent/child guides and within the same guide before adding new material.
- Prefer one dense, readable sentence over multiple bullets when the facts are inseparable.
- Use symbols, paths, and commands as retrieval anchors, not as a source tour. Avoid `currently`,
  `today`, and version snapshots unless the version itself is an active compatibility boundary.
- Omit project introductions, implementation narration, concrete examples, pending fixes, local
  workarounds, transient status, speculation, logs, and details better kept in source comments,
  tests, plans, or issues.
- Preserve existing notes only while they remain true, scoped, and actionable; update corrected
  boundaries such as deprecated areas.

### 5. Audit Existing Notes

For an explicitly requested full audit:

- Enumerate every guide with `rg --files -g AGENTS.md` and inspect every note; do not treat an
  unchanged file as implicitly audited.
- Classify each note as keep, rewrite, move, merge, or remove based on truth, usefulness, scope, and
  agent actionability.

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
- A section explains the product or subsystem to a person without changing an agent's next action.
- A bullet provides background but no trigger, constraint, owner, action, or evidence gate.
- A new scoped AGENTS.md contains only one narrow observation.
- The same rule appears in parent and child scopes without a strong independent-trigger reason.
- Notes mention specific attempt outcomes, timings, TODO fixes, or symbol mechanics that source comments/tests should own.
- A path, command, macro, or ownership claim was copied without exact verification.
- A note is justified only by "this mattered during the session" or "this is what the code currently looks like."
