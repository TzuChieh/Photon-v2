# RenderTest Guide

Goal: prevent subtle test/report regressions with simple, deterministic rules.

## Run Isolation
- Treat each run as fresh.
- Never let previous artifacts affect current report correctness.

## Model-First Reports
- Report data model is the source of truth (case JSON + run meta).
- Markdown/HTML are pure projections of that model.

## Deterministic State
- Recompute from current inputs instead of long-lived cached UI state.
- Remove state that is written but never read.
- Persist viewer selections by semantic identity (e.g., verifier name), not fragile transient indexes.

## Stable UX Surface
- Keep primary controls spatially stable during repeated navigation.
- Avoid reflow that moves controls unexpectedly.
- For compare overlays on arbitrary images, prefer dual-tone separators (light + dark) for robust visibility.

## Explicit Semantics
- Comparison direction must be explicit and consistent (e.g. slider side meaning).
- Field names should encode intent (`plot_*` vs `raw_*`).

## Simple Lifecycle Boundaries
- Keep local report-server control explicit and minimal; report viewers should serve already generated artifacts.
- Prefer explicit stop signals over speculative background policies.
- Stop `report_server.py` before regenerating reports if `report_output` cleanup is file-locked; stale report files must not be preserved to work around the lock.
- Do not rely on browser unload semantics to distinguish close vs refresh/navigation.
- For report UI-only changes and test case updates, sync source template and build-side copy so `--report-only` validates quickly.
- Use `--test-only` only for validation; if the user needs to view a report, run without it or use `--report-only` only after confirming case JSON exists.

## Sample Count Semantics
- `ZTestVerifier(sample_count=...)` is the tested render's independent sample count; the z statistic divides reference variance by this value.
- For path tracing tests, keep `sample_count` aligned with the scene's `sample-source(...)[integer samples N]`.
- Rendering methods may share beauty reference scenes/images when the expected radiance is the same; do not create method-specific beauty reference scenes just to vary the tested method.
- Variance references are keyed by sampling distribution, not by test row. Share them for intentionally equivalent variants such as thread count, scene format, or rigid scene shifts; split `bvpt`/`bneept`, material, geometry, lighting, filter, and renderer-config variants unless equivalence is justified.
- Do not use `ZTestVerifier` for photon-mapping methods. Their progressive biased estimators do not share the independent-sample variance model used by path tracing.
- Scenes, fixtures, and references come from the separate `Photon-v2-Resource` repo through the
  ignored `build/Photon-v2-Resource/` setup copy; after geometry, framing, or resolution changes,
  regenerate references in the source repo, sync the build copy when not rerunning setup, and
  inspect raw output at the intended dimensions.
- Prefer stronger references and appropriate tested sample counts before loosening verifier tolerances; record metric/threshold usage when retuning.
- Reference image stems should use semantic suffixes (`_beauty`, `_var`). Keep batch ref generation explicit with a scene-to-output map that matches the active ref set.
- For `ZTestVerifier`, the reference scene must explicitly render a variance film; declaring `"var"` in the ref-output map only validates expected files and does not make the renderer produce variance output.

## Scene Resource Organization
- Keep each RenderTest resource folder standalone. Split repeated SDL into render-config roots and local data fragments; do not create cross-folder common scenes or extract small one-off scenes.
- Prefer one direct `#import` per scene root. Avoid nesting unless a substantial local base is shared by multiple variants, such as `gray_furnace_box/large_furnace_box.p2`.

## CLI Safety
- `run_and_report.py` currently parses with `parse_known_args()` and forwards unknown args to `pytest.main()`.
- Cleanup currently happens after RenderTest-owned args are parsed and before `pytest.main()`; `--report-only` skips cleanup.
- Unknown argument typos can still trigger cleanup before `pytest` rejects them; do not document typo protection unless the parser rejects or explicitly separates pytest args first.

## Report UX
- Avoid duplicating verifier status/name in the main image area when verifier buttons already show name and state.
- Keep filtered report navigation coherent: side lists, dropdowns, buttons, and keyboard shortcuts should share the same visible test/case set.
- Keep high-value image space clear; move auxiliary hints to compact controls/tooltips.
- Keep top controls spatially stable. Test/case selectors and verifier/view controls should split the header evenly on desktop.
- For focused slider compare, avoid scaling the `img-comparison-slider` element itself; resize it so its internal drag math remains correct.
