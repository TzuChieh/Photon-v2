---
name: committer
description: Photon-v2 committer for code review and git staging. Use when you need to review staged changes, stage files with "git add -A", and draft concise, impactful commit messages for non-master branches.
---

# Committer

This skill provides a specialized workflow for high-quality code reviews and commits in the Photon-v2 project.

## Core Workflow

- **Initial Work:** Keep the initial implementation of a task UNSTAGED for the first review.
- **Incremental Review Staging:** Stage your current work (`git add -A`) ONLY when the user asks for an update or correction. This allows the user to see your latest changes as a "delta" in the unstaged section of their IDE, while the previous version remains in the staged section.
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
- **Concise Message:** Draft a short, clear message (e.g., "Add documentation for SdlFunction") that focuses on the core change. 
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
