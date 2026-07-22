# RenderTest Guide

## Run and Report Model
- Start each test run from fresh outputs; never let prior artifacts affect case or report
  correctness.
- Treat case JSON and `test_meta.json` as the report source of truth; regenerate Markdown/HTML
  projections and derived UI state from those inputs.
- Persist viewer selections by semantic identity, such as verifier name, rather than transient
  indexes.

## Report Lifecycle
- Keep `report_server.py` as an explicit server for already generated artifacts.
- Stop `report_server.py` before regenerating reports if `report_output` cleanup is file-locked;
  never preserve stale report files to work around the lock.
- Do not rely on browser unload semantics to distinguish close vs refresh/navigation.
- For report UI-only changes and test case updates, sync source template and build-side copy so `--report-only` validates quickly.
- Use `--test-only` only for validation; if the user needs to view a report, run without it or use `--report-only` only after confirming case JSON exists.

## Sample Count Semantics
- Set `ZTestVerifier(sample_count=...)` to the tested render's independent sample count and keep it
  aligned with the path-tracing scene's `sample-source(...)[integer samples N]`; the verifier
  divides reference variance by this value.
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
- Treat output cleanup as destructive: validate RenderTest-owned and forwarded pytest arguments
  before deleting outputs, and keep `--report-only` non-cleaning.

## Report UX
- Keep top controls spatially stable and filtered navigation coherent across side lists, dropdowns,
  buttons, and keyboard shortcuts.
- Keep image space free of duplicate verifier status/name and move auxiliary hints to compact
  controls or tooltips.
- Keep comparison direction and `plot_*` versus `raw_*` field intent explicit; use dual-tone
  separators for arbitrary images, and resize rather than scale `img-comparison-slider` so its drag
  math remains correct.
