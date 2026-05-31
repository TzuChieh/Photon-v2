# Agent Skills Guide

## Layout
- Skill sources live under `Main/AgentSkills/<Skill>/`. Sibling `Main/AgentSkills/*.skill` ZIP packages are ignored by git; regenerate local packages after source edits when needed.
- Keep `SKILL.md` concise. Put detailed reusable guidance in linked `references/` files only when it earns the indirection.

## Writing
- Frontmatter descriptions drive activation. State user intent and important near-miss boundaries explicitly.
- Prefer project-specific workflows and gotchas over generic engineering advice. Defer to repo and scoped `AGENTS.md` rules.
- Repeat a short critical invariant across independently invoked skills when that prevents mistakes; avoid duplicating long guidance.
