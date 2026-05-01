import infra
from infra import paths
import pytest

res_dir = paths.test_resources() / "fullscreen_unit_radiance"

suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=1)
verifiers = [
    infra.VisualErrorVerifier(ref=1.0, error_scale=100.0),
    infra.PerPixelVerifier(expected_value=1.0, tolerance=1e-8)
]

suite.add_case(infra.RenderCase("BVPT", res_dir / "scene_bvpt.p2", renderer_config, verifiers))
suite.add_case(infra.RenderCase("BNEEPT", res_dir / "scene_bneept.p2", renderer_config, verifiers))
suite.add_case(infra.RenderCase("SPPM", res_dir / "scene_sppm.p2", renderer_config, verifiers))

@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    These scenes are all arranged in a way that they should render as a frame filled with
    white of unit magnitude (1, 1, 1). All output images should appear white, and all images showing
    error should be completely black (or any other color representing 0, depending on the color map).
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
