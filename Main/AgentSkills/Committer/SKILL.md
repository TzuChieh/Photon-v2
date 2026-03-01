---
name: committer
description: Photon-v2 committer for code review and git staging. Use when you need to review staged changes, stage files with "git add -A", and draft concise, impactful commit messages for non-master branches.
---

# Committer

This skill provides a specialized workflow for high-quality code reviews and commits in the Photon-v2 project.

## Core Workflow

### 1. Code Review (Sanity Check)
Before staging, use `git diff` to perform a quick review based on the project's coding standards:
- **No `new` or `delete`:** Ensure objects are managed by smart pointers or RAII.
- **`const` Correctness:** Check if `const` is applied to variables and methods where intended.
- **Formatting:** Verify curly braces have their own line and operators are surrounded by spaces.
- **Naming:** Confirm `m_` prefix for members and capitalized camel case for classes.
- **Minimalism:** Ensure no "just-in-case" code or redundant logic is included.

### 2. Staging & Draft
- **Git Add:** Use `git add -A` to stage all changes in the current project.
- **Concise Message:** Draft a short, clear message (e.g., "Add documentation for SdlFunction") that focuses on the core change. 
- **Branch Check:** If the current branch is not `master`, proceed with the commit. If it is `master`, suggest running tests (`EngineTest`, `RenderTest`) before finality.

### 3. Execution
Apply the commit using:
```bash
git add -A && git commit -m "[Your concise message]"
```

## References
- [review-checklist.md](references/review-checklist.md): A technical checklist for code quality.
- [Main/Documentation/coding_standard.md](../Main/Documentation/coding_standard.md): The project's overall coding standard.
