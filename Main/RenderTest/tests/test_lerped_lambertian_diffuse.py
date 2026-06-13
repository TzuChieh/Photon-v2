import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "lerped_lambertian_diffuse"
suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=4)
ref_path = res_dir / "ref_no_lerp_bvpt_65536spp_beauty"
factor0p0_bvpt_ref_var_path = res_dir / "ref_no_lerp_bvpt_65536spp_var"
factor0p5_bvpt_ref_var_path = res_dir / "ref_factor0p5_bvpt_65536spp_var"
factor0p8_bvpt_ref_var_path = res_dir / "ref_factor0p8_bvpt_65536spp_var"
factor0p0_bneept_ref_var_path = res_dir / "ref_no_lerp_bneept_65536spp_var"
factor0p5_bneept_ref_var_path = res_dir / "ref_factor0p5_bneept_65536spp_var"

def output_title(case, verifier_reports):
    return "%s Output (MSE: %f, Δ: %f%%)" % (
        case.name,
        infra.get_metric(verifier_reports, infra.MSEVerifier.METRIC_MSE),
        infra.get_metric(verifier_reports, infra.RelMeanVerifier.METRIC_REL_MEAN) * 100)

visual_error_verifier = infra.VisualErrorVerifier(
    ref=ref_path,
    error_scale=100.0,
    ref_title="Reference: BVPT (no lerp) 65536 spp",
    color_max=70)

for case_name, output_name, scene_name, ref_var_path, max_mse, max_rel_mean, case_msg in [
    ("BVPT (factor = 50 percent)", "bvpt_factor0p5", "scene_factor0p5_bvpt.p2", factor0p5_bvpt_ref_var_path, 0.00012, 0.00054, ""),
    ("BVPT (factor = 80 percent)", "bvpt_factor0p8", "scene_factor0p8_bvpt.p2", factor0p8_bvpt_ref_var_path, 0.00012, 0.0001, ""),
    ("BVPT (factor = 0 percent)", "bvpt_factor0p0", "scene_factor0p0_bvpt.p2", factor0p0_bvpt_ref_var_path, 0.00012, 0.00074, "This is a corner case where the lerping factor is 0 (0 * material_0 + 1 * material_0)."),
    ("BNEEPT (factor = 50 percent)", "bneept_factor0p5", "scene_factor0p5_bneept.p2", factor0p5_bneept_ref_var_path, 0.00012, 0.00080, ""),
    ("BNEEPT (factor = 0 percent)", "bneept_factor0p0", "scene_factor0p0_bneept.p2", factor0p0_bneept_ref_var_path, 0.00012, 0.00012, "This is a corner case where the lerping factor is 0 (0 * material_0 + 1 * material_0).")
    ]:
    suite.add_case(infra.RenderCase(
        case_name,
        res_dir / scene_name,
        renderer_config,
        [
            infra.MSEVerifier(ref=ref_path, threshold=max_mse),
            infra.RelMeanVerifier(ref=ref_path, threshold=max_rel_mean),
            visual_error_verifier,
            infra.ZTestVerifier(ref=ref_path, ref_variance=ref_var_path, sample_count=4000)
        ],
        output_filename=output_name,
        output_title=output_title,
        case_msg=case_msg))

@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    A sphere with lerped Lambertian diffuse of different factors, comparing against a non-lerped (albedo = 
    100%) reference. The ground is non-lerped diffusive (albedo = 50%).
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
