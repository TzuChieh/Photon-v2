import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "white_100W_point_light"
suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=4)
ref_path = res_dir / "ref_bneept_65536spp_0"
ref_var_path = res_dir / "ref_bneept_65536spp_1"

def output_title(case, metrics):
    return "%s Output (MSE: %f, Δ: %f%%)" % (case.name, metrics["mse"], metrics["rel_mean"] * 100)

visual_error_verifier = infra.VisualErrorVerifier(
    ref=ref_path,
    error_scale=100.0,
    ref_title="Reference: BNEEPT 65536 spp")

for case_name, output_name, scene_name, sample_count, max_mse, max_rel_mean in [
    ("BNEEPT", "bneept", "scene_bneept.p2", 5000, 0.000036, 0.001),
    ("SPPM", "sppm", "scene_sppm.p2", 1000, 0.000169, 0.012)
    ]:
    verifiers = [
        infra.MSEVerifier(ref=ref_path, threshold=max_mse),
        infra.RelMeanVerifier(ref=ref_path, threshold=max_rel_mean),
        visual_error_verifier,
        infra.ZTestVerifier(ref=ref_path, ref_variance=ref_var_path, sample_count=sample_count)]

    suite.add_case(infra.RenderCase(
        case_name,
        res_dir / scene_name,
        renderer_config,
        verifiers,
        output_filename=output_name,
        output_title=output_title))

@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    This test is similar to the "white_100W_small_rect_area_light" test, except that we are using a
    point light here. In Photon, we do not have true point light, they are just tiny spherical light.
    This test is not suitable to run using BVPT, as it may require way more than 10M samples to have
    proper convergence.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
