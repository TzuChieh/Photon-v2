import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "sample_generators"
suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=3)
ref_path = res_dir / "ref_bvpt_1000spp_beauty"

verifiers = [
    infra.MSEVerifier(ref=ref_path, threshold=1e-7),
    infra.RelMeanVerifier(ref=ref_path, threshold=1e-6),
    infra.VisualErrorVerifier(ref=ref_path, error_scale=100.0)
]

for case_name, scene_name in [
    ("Stratified", "scene_bvpt_stratified.p2"),
    ("Halton", "scene_bvpt_halton.p2"),
    ("Uniform Random", "scene_bvpt_uniform_random.p2")
]:
    suite.add_case(infra.RenderCase(
        case_name,
        res_dir / scene_name,
        renderer_config,
        verifiers,
        output_filename=case_name.lower().replace(" ", "_")))


@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    A camera-facing emissive image should render the same expected image regardless of sample
    generator. More varied generators use more samples to keep stochastic edge errors bounded.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
