import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "white_100W_small_rect_area_light"
suite = infra.RenderTestSuite(__name__, res_dir)

ref_path = res_dir / "ref_bneept_4096spp_0"
ref_var_path = res_dir / "ref_bneept_4096spp_1"

def output_title(case, metrics):
    return "%s Output (MSE: %f, Δ: %f%%)" % (case.name, metrics["mse"], metrics["rel_mean"] * 100)

visual_error_verifier = infra.VisualErrorVerifier(
    ref=ref_path,
    error_scale=100.0,
    ref_title="Reference: BNEEPT 4096 spp")

for case_name, output_name, scene_name, z_sample_count, max_mse, max_rel_mean in [
    ("BNEEPT", "bneept", "scene_bneept.p2", 512, 0.000004, 0.00022),
    ("SPPM", "sppm", "scene_sppm.p2", None, 0.0025, 0.006)
    ]:
    num_threads = 10 if "SPPM" in case_name else 4
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
        output_title=output_title))

@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    This test is similar to the "white_100W_rect_area_light" test, except that the rectangular
    area light is much smaller (0.005 x 0.005 unit^2), and the light source is placed higher above the
    ground so it cannot induce too much variance (it is a strong radiance source). This test is not
    suitable to run using  BVPT, as it may require way more than 10M samples to have proper convergence.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
