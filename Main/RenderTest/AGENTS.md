# RenderTest Principles (`Main/RenderTest`)

Goal: prevent subtle test/report regressions with simple, deterministic rules.

## 1) Run Isolation
- Treat each run as fresh.
- Never let previous artifacts affect current report correctness.

## 2) Model-First Reports
- Report data model is the source of truth (case JSON + run meta).
- Markdown/HTML are pure projections of that model.

## 3) Deterministic State
- Recompute from current inputs instead of long-lived cached UI state.
- Remove state that is written but never read.
- Persist viewer selections by semantic identity (e.g., verifier name), not fragile transient indexes.

## 4) Stable UX Surface
- Keep primary controls spatially stable during repeated navigation.
- Avoid reflow that moves controls unexpectedly.
- For compare overlays on arbitrary images, prefer dual-tone separators (light + dark) for robust visibility.

## 5) Explicit Semantics
- Comparison direction must be explicit and consistent (e.g. slider side meaning).
- Field names should encode intent (`plot_*` vs `raw_*`).

## 6) Simple Lifecycle Boundaries
- Keep local report-server control explicit and minimal.
- Prefer explicit stop signals over speculative background policies.
- Do not rely on browser unload semantics to distinguish close vs refresh/navigation.
- For report UI-only changes, sync source template and build-side copy so `--report-only` validates quickly.
