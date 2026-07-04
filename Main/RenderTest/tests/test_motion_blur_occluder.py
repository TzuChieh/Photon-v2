import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "motion_blur_occluder"
suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=4)
ref_path = res_dir / "ref_bvpt_32768spp_beauty"
ref_var_path = res_dir / "ref_bvpt_32768spp_var"

verifiers = [
    infra.MSEVerifier(ref=ref_path, threshold=0.0005),
    infra.RelMeanVerifier(ref=ref_path, threshold=0.002),
    infra.ZTestVerifier(ref=ref_path, ref_variance=ref_var_path, sample_count=3000),
    infra.VisualErrorVerifier(ref=ref_path, error_scale=100.0)
]

suite.add_case(infra.RenderCase(
    "BVPT",
    res_dir / "scene_bvpt.p2",
    renderer_config,
    verifiers,
    output_filename="bvpt"))


@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    A black sphere sweeps across a directly visible white emitter. The expected image has a
    capsule-like motion-blur silhouette from primary-ray time sampling.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
