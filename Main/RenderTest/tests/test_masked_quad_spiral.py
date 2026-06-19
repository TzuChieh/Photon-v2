import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "masked_quad_spiral"
suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=4)
ref_path = res_dir / "ref_bvpt_32768spp_beauty"
ref_var_path = res_dir / "ref_bvpt_32768spp_var"

suite.add_case(infra.RenderCase(
    "BVPT",
    res_dir / "scene_bvpt.p2",
    renderer_config,
    [
        infra.MSEVerifier(ref=ref_path, threshold=0.0004),
        infra.VisualErrorVerifier(
            ref=ref_path,
            error_scale=10.0,
            ref_title="Reference: BVPT 32768 spp"),
        infra.ZTestVerifier(ref=ref_path, ref_variance=ref_var_path, sample_count=512)
    ],
    output_filename="bvpt"))


@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    A masked quad sits between the camera and a large directly visible area light.
    The mask is a spiral gradient, which should reveal a clear spiral-shaped light pattern.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
