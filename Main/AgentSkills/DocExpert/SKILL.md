---
name: doc-expert
description: Documentation writing expert for C++ code in Photon-v2. Use when you need to find undocumented code, update stale documentation, or improve doc clarity. It uses git to prioritize work based on recent changes and call frequency.
---

# Doc-Expert

This skill provides a specialized workflow for maintaining high-quality Doxygen documentation in the Photon-v2 project.

## Core Workflow

### 1. Identify Target Areas
- **Git-Driven Selection:** Use `git diff HEAD` or `git status` to find files you have recently modified. Focus on public headers (`.h`) first.
- **Priority by Usage:** For undocumented or changed symbols, use `grep_search` to count occurrences across the codebase. Higher frequency symbols (more call sites) take priority.
- **Stale Check:** Compare Doxygen `@param` and `@return` tags against the actual function signature. Flag any mismatches.

### 2. Analysis & Drafting
- **Verify Intent:** Read the implementation (`.cpp`) to understand side effects, units, and corner cases.
- **Drafting:** Use `/*! @brief ... */` for summaries. Use `@param` for all parameters and `@return` for non-void functions.
- **Clarity Check:** Ensure descriptions are concise and technically precise. Mention units (e.g., "Radiance in $W \cdot sr^{-1} \cdot m^{-2}$") and coordinate systems where relevant.

### 3. Execution
- Apply changes using the `replace` tool.
- Follow the formatting in `doc-expert/references/style-examples.md`.

### 4. Human Consultation
**Stop and ask the user if:**
- You are unsure of a parameter's purpose after reading the code.
- A mathematical formula or paper reference is missing but seems necessary for clarity.
- You encounter ambiguous legacy code where "correctness" is unclear.

## References
- [style-examples.md](references/style-examples.md): Gold-standard documentation snippets from the project.
- [Main/Documentation/coding_standard.md](../Main/Documentation/coding_standard.md): The project's overall coding standard.
