import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "white_100W_rect_area_light"
suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=4)
ref_path = res_dir / "ref_bneept_8192spp"

def output_title(case, metrics):
    return "%s Output (MSE: %f, Δ: %f%%)" % (case.name, metrics["mse"], metrics["rel_mean"] * 100)

visual_error_verifier = infra.VisualErrorVerifier(
    ref=ref_path,
    error_scale=100.0,
    ref_title="Reference: BNEEPT 8192 spp")

for case_name, output_name, scene_name, max_mse, max_rel_mean in [
    ("BVPT", "bvpt", "scene_bvpt.p2", 0.0036, 0.002),
    ("BNEEPT", "bneept", "scene_bneept.p2", 0.00152, 0.002),
    ("SPPM", "sppm", "scene_sppm.p2", 0.0058, 0.0034)
    ]:
    suite.add_case(infra.RenderCase(
        case_name,
        res_dir / scene_name,
        renderer_config,
        [
            infra.MSEVerifier(ref=ref_path, threshold=max_mse),
            infra.RelMeanVerifier(ref=ref_path, threshold=max_rel_mean),
            visual_error_verifier
        ],
        output_filename=output_name,
        output_title=output_title))

@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    A rectangular area light is shining the ground below it. The ground is diffusive (albedo = 50%).
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
