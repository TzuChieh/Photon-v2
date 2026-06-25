# Agent Skills Guide

## Layout
- Skill sources live under `Main/AgentSkills/<Skill>/`. Sibling `Main/AgentSkills/*.skill` ZIP packages are ignored by git; regenerate local packages after source edits when needed.
- Setup recreates ignored `.agents/skills` as a directory link to `Main/AgentSkills`; keep durable skill content in `Main/AgentSkills`, not under `.agents`.
- Keep `SKILL.md` concise. Put detailed reusable guidance in linked `references/` files only when it earns the indirection.

## Profiling Evidence
- Keep profiling artifacts in the relevant ignored `Generated/` folder: workload copies, timing logs, reports, and target assembly listing paths or minimal excerpts.
- For low-level optimization skills, compare project-generated listings from the measured target library or executable; verify timestamps and the live translation unit before trusting assembly after source moves.
- Do not infer call overhead from VTune symbol names alone; MSVC may inline operations while samples stay attributed to helper/library names, so confirm the hot range with disassembly before keeping a patch.
- For renderer optimization claims, use the renderer's finished-render log time from the same scene/thread count/configuration, disable assertions unless they are the target, compare repeated runs by minimum time, and exclude runs with known contention or interruption.
- For optimization sweeps, keep a full attempt ledger plus a concise top-10 summary; revert rejected source changes immediately and refresh Git status so content-clean files do not remain dirty.
- Treat API rewrites, direct constructors, and cached derived-data changes as hypotheses; keep them only when renderer timing plus target assembly or VTune evidence improves over the current kept state.

## Writing
- Frontmatter descriptions drive activation. State user intent and important near-miss boundaries explicitly.
- Prefer project-specific workflows and gotchas over generic engineering advice. Defer to repo and scoped `AGENTS.md` rules.
- Repeat a short critical invariant across independently invoked skills when that prevents mistakes; avoid duplicating long guidance.
