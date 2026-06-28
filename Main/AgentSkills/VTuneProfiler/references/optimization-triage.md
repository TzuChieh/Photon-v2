# Optimization Triage

Use this when VTune profiling leads to source changes, candidate patches, or speedup claims.

## Baseline

- Use the same scene, thread count, renderer settings, build type, and output path shape for baseline and candidates.
- Prefer profiling builds with `PH_DEBUG=OFF`, `PH_STATS=OFF`, and high-cost stats off unless the task targets those systems.
- Save workload copies, timing logs, VTune reports, candidate patches, and target assembly under `Main\AgentSkills\VTuneProfiler\Generated\<case>\`.
- Compare repeated renderer finished-render times by minimum time, and exclude runs with known contention or interruption.

## Patch Selection

- Optimize measured clusters, not isolated helper names. Treat VTune attribution as a lead until the hot instruction range is confirmed.
- Prefer small local changes that remove measured work while preserving the existing API shape; reject broad rewrites without full-render timing evidence.
- For MSVC generic math hot paths, fixed-extent direct expressions can be valid when assembly shows dynamic indexing, stack materialization, spills, or avoidable loop scaffolding.
- Keep simple per-element loops such as finite checks unless assembly and render timing prove the compiler is missing an optimization.
- When `normalize()` is hot through `length()`, optimize the shared length helper first instead of duplicating normalization math at call sites.
- For default-channel texture or hit-detail paths, avoid redundant channel switching and `HitDetail` copies when channel 0 is already the sampled channel.
- Treat direct constructors, extra inlining, cached derived data, and row-pointer/index-hoist rewrites as hypotheses; keep them only when renderer timing and codegen agree.

## Attempt Ledger

- Record each attempt with intent, touched hot path, expected mechanism, timing result, speedup/slowdown, evidence used, and keep/revert decision.
- Revert rejected source changes immediately and refresh Git status; keep patch files and notes in `Generated` rather than leaving dirty source files.
- Be skeptical of sub-1% changes unless they repeat cleanly and match an assembly or VTune explanation.
