---
name: committer
description: Use this skill when the user wants to prepare or create a git commit, including reviewing commit scope, staging approved changes, or drafting a commit message. Do not use it for standalone code review without commit preparation.
---

# Committer

Follow the repo and scoped `AGENTS.md` rules first. Git is read-only unless the user explicitly requests staging or committing.

## Workflow

### 1. Review

- Inspect `git status`, unstaged and staged diffs, and relevant scoped `AGENTS.md` files.
- Prioritize correctness, regressions, unintended files, and missing tests. Use [review-checklist.md](references/review-checklist.md) and the [coding standard](../../Documentation/coding_standard.md).
- Report blocking findings before staging or committing.

### 2. Stage and Draft

- Stage only after an explicit user request. Confirm the intended scope; use `git add -A` only when the request covers all changes.
- Review the staged diff and draft a concise one-line imperative summary. After a blank line, add short bullet points only when they carry useful context.

### 3. Commit

- Require one explicit `LGTM` or equivalent for each `git commit`. One approval permits exactly one commit.
- Do not push unless explicitly requested. Run PowerShell commands sequentially without `&&` or `;`.
- Report the commit hash, message, and any verification not run.

## Project Rule

Do not build or run binaries or tests; the user handles execution.
