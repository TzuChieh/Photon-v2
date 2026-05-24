import infra
from infra import paths
import pytest

res_dir = paths.test_resources() / "glossy_plane"

suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=10)
ref_path = res_dir / "ref_bvpt_1048576spp_0"
ref_var_path = res_dir / "ref_bvpt_1048576spp_1"

def output_title(case, metrics):
    return "%s Output (MSE: %f, Δ: %f%%)" % (case.name, metrics["mse"], metrics["rel_mean"] * 100)

visual_error_verifier = infra.VisualErrorVerifier(
    ref=ref_path,
    error_scale=100.0,
    ref_title="Reference: BVPT 1048576 spp")

suite.add_case(infra.RenderCase(
    "BVPT",
    res_dir / "scene_bvpt.p2",
    renderer_config,
    [
        infra.MSEVerifier(ref=ref_path, threshold=0.0036),
        infra.RelMeanVerifier(ref=ref_path, threshold=0.0056),
        visual_error_verifier,
        infra.ZTestVerifier(ref=ref_path, ref_variance=ref_var_path, sample_count=1500)
    ],
    output_title=output_title))
suite.add_case(infra.RenderCase(
    "BNEEPT",
    res_dir / "scene_bneept.p2",
    renderer_config,
    [
        infra.MSEVerifier(ref=ref_path, threshold=0.003),
        infra.RelMeanVerifier(ref=ref_path, threshold=0.0034),
        visual_error_verifier,
        infra.ZTestVerifier(ref=ref_path, ref_variance=ref_var_path, sample_count=800)
    ],
    output_title=output_title))

@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    A glossy plane is placed in front of the camera. The plane is illuminated by a small
    rectangular area light. The reflection of the light can be seen on the plane.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
