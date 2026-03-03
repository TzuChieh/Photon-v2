---
name: committer
description: Photon-v2 committer for code review and git staging. Use when you need to review staged changes, stage files with "git add -A", and draft concise, impactful commit messages for non-master branches.
---

# Committer

This skill provides a specialized workflow for high-quality code reviews and commits in the Photon-v2 project.

## Core Workflow

- **Stage Before Update:** Always stage your current changes (`git add -A`) BEFORE applying any new updates or corrections. This ensures the user can see your latest changes as an unstaged "delta" in their IDE (like VSCode), while the previous version remains in the staged section.
- **Initial Work:** Keep the initial implementation of a task UNSTAGED for the first review.
- **No Unauthorized Commits:** NEVER commit changes without explicit user consent (e.g., "LGTM", "Proceed with commit").
- **No Push:** Do not push changes to a remote repository unless specifically requested.

### 1. Code Review (Sanity Check)
Before staging, use `git diff` to perform a quick review based on the project's coding standards and **modern C++ best practices**:
- **Resource Management:** No `new` or `delete`; use RAII and smart pointers.
- **Type Safety:** Use `nullptr`, strongly-typed enums (`enum class`), and `auto` only where it improves clarity.
- **`const` Correctness:** Apply `const` and `constexpr` rigorously to variables and methods.
- **Modern Features:** Leverage C++20/23 features (concepts, ranges, etc.) when they simplify logic.
- **Formatting & Naming:** Follow the `m_` prefix for members and PascalCase for classes.
- **Minimalism:** Ensure no "just-in-case" code or redundant logic is included.

### 2. Staging & Draft
- **Git Add:** Use `git add -A` to stage all changes in the current project.
- **Linux Style Message:** Use a concise one-line summary (e.g., "Add documentation for SdlFunction"), followed by a blank line and bullet points for additional details if necessary.
- **Branch Check:** If the current branch is not `master`, proceed with the commit. If it is `master`, suggest running tests (`EngineTest`, `RenderTest`) before finality.

### 3. Execution
Apply the commit using:
```bash
git add -A; git commit -m "[Your concise message]"
```
*(Note: Use `;` on Windows/PowerShell for command chaining.)*

## References
- [review-checklist.md](references/review-checklist.md): A technical checklist for code quality.
- [Main/Documentation/coding_standard.md](../Main/Documentation/coding_standard.md): The project's overall coding standard.
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines): The industry standard for modern C++ best practices.
