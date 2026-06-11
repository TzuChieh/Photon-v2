import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "single_ply_mesh"
suite = infra.RenderTestSuite(__name__, res_dir)

quad_ref_path = res_dir / "ref_quad_bvpt_16384spp_beauty"
suzanne_ref_path = res_dir / "ref_suzanne_bneept_16384spp_beauty"
quad_bvpt_ref_var_path = res_dir / "ref_quad_bvpt_16384spp_var"
quad_bneept_ref_var_path = res_dir / "ref_quad_bneept_16384spp_var"
suzanne_bvpt_ref_var_path = res_dir / "ref_suzanne_bvpt_16384spp_var"
suzanne_bneept_ref_var_path = res_dir / "ref_suzanne_bneept_16384spp_var"

sppm_msg = (
    "This SPPM case intentionally uses the Blackman-Harris sample filter because "
    "the reference image was rendered with the same filter. This keeps boundary "
    "and silhouette filtering comparable to the reference, even though SPPM photon "
    "statistics are accumulated per pixel/sub-pixel strata and are most naturally "
    "matched with a box filter."
    )

def output_title(case, verifier_reports):
    return "%s Output (MSE: %f, Δ: %f%%)" % (
        case.name,
        infra.get_metric(verifier_reports, infra.MSEVerifier.METRIC_MSE),
        infra.get_metric(verifier_reports, infra.RelMeanVerifier.METRIC_REL_MEAN) * 100)

quad_visual_error_verifier = infra.VisualErrorVerifier(
    ref=quad_ref_path,
    error_scale=100.0,
    ref_output_filename="ref_quad",
    ref_title="Reference: BVPT 16384 spp")
suzanne_visual_error_verifier = infra.VisualErrorVerifier(
    ref=suzanne_ref_path,
    error_scale=100.0,
    ref_output_filename="ref_suzanne",
    ref_title="Reference: Suzanne 16384 spp")

for case_name, output_name, scene_name, num_threads, ref_path, ref_var_path, visual_error_verifier, z_sample_count, max_mse, max_rel_mean in [
    ("Quad (BVPT, ASCII)", "quad_bvpt_ascii", "quad_bvpt_ascii.p2", 
     6, quad_ref_path, quad_bvpt_ref_var_path, quad_visual_error_verifier, 200, 0.000074, 0.0015),
    ("Quad (BVPT)", "quad_bvpt", "quad_bvpt.p2", 
     6, quad_ref_path, quad_bvpt_ref_var_path, quad_visual_error_verifier, 200, 0.000074, 0.0015),
    ("Quad (BNEEPT, ASCII)", "quad_bneept_ascii", "quad_bneept_ascii.p2", 
     6, quad_ref_path, quad_bneept_ref_var_path, quad_visual_error_verifier, 200, 0.000074, 0.0014),
    ("Quad (BNEEPT)", "quad_bneept", "quad_bneept.p2", 
     6, quad_ref_path, quad_bneept_ref_var_path, quad_visual_error_verifier, 200, 0.000074, 0.0014),
    ("Quad (SPPM)", "quad_sppm", "quad_sppm.p2", 
     6, quad_ref_path, None, quad_visual_error_verifier, None, 0.0014, 0.01),
    ("Suzanne (BVPT)", "suzanne_bvpt", "suzanne_bvpt.p2", 
     12, suzanne_ref_path, suzanne_bvpt_ref_var_path, suzanne_visual_error_verifier, 200, 0.00007, 0.001),
    ("Suzanne (BNEEPT)", "suzanne_bneept", "suzanne_bneept.p2", 
     12, suzanne_ref_path, suzanne_bneept_ref_var_path, suzanne_visual_error_verifier, 200, 0.00007, 0.00026),
    ("Suzanne (SPPM)", "suzanne_sppm", "suzanne_sppm.p2", 
     12, suzanne_ref_path, None, suzanne_visual_error_verifier, None, 0.00009, 0.0045)
    ]:
    verifiers = [
        infra.MSEVerifier(ref=ref_path, threshold=max_mse),
        infra.RelMeanVerifier(ref=ref_path, threshold=max_rel_mean),
        visual_error_verifier]
    if z_sample_count is not None:
        verifiers.append(infra.ZTestVerifier(
            ref=ref_path, ref_variance=ref_var_path, sample_count=z_sample_count))

    suite.add_case(infra.RenderCase(
        case_name,
        res_dir / scene_name,
        infra.RendererConfig(num_threads=num_threads),
        verifiers,
        output_filename=output_name,
        output_title=output_title,
        case_msg=sppm_msg if output_name in {"quad_sppm", "suzanne_sppm"} else ""))

@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    A single .ply mesh is placed between a large area light and diffusive ground. The mesh itself is diffusive.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
