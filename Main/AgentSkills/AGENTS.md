# Agent Skills Guide

## Layout
- Skill sources live under `Main/AgentSkills/<Skill>/`. Sibling `Main/AgentSkills/*.skill` ZIP packages are ignored by git; regenerate local packages after source edits when needed.
- Setup recreates ignored `.agents/skills` as a directory link to `Main/AgentSkills`; keep durable skill content in `Main/AgentSkills`, not under `.agents`.
- Keep `SKILL.md` concise. Put detailed reusable guidance in linked `references/` files only when it earns the indirection.

## Profiling Evidence
- Keep profiling artifacts in the relevant ignored `Generated/` folder: workload copies, timing logs, reports, and target assembly listing paths or minimal excerpts.
- For low-level optimization skills, compare project-generated listings from the measured target library or executable. Prefer the original compiled body on the hot path; create standalone repro code only when the target build cannot expose the needed assembly.

## Writing
- Frontmatter descriptions drive activation. State user intent and important near-miss boundaries explicitly.
- Prefer project-specific workflows and gotchas over generic engineering advice. Defer to repo and scoped `AGENTS.md` rules.
- Repeat a short critical invariant across independently invoked skills when that prevents mistakes; avoid duplicating long guidance.
