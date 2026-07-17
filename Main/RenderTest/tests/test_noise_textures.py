import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "noise_textures"
suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=4)
ref_path = res_dir / "ref_bvpt_8192spp_beauty"
ref_var_path = res_dir / "ref_bvpt_8192spp_var"


def output_title(case, verifier_reports):
    return "%s Output (MSE: %f)" % (
        case.name,
        infra.get_metric(verifier_reports, infra.MSEVerifier.METRIC_MSE))


suite.add_case(infra.RenderCase(
    "BVPT",
    res_dir / "scene_bvpt.p2",
    renderer_config,
    [
        infra.MSEVerifier(ref=ref_path, threshold=0.00001),
        infra.VisualErrorVerifier(
            ref=ref_path,
            error_scale=100.0,
            ref_title="Reference: BVPT 8192 spp"),
        infra.ZTestVerifier(
            ref=ref_path,
            ref_variance=ref_var_path,
            sample_count=512)
    ],
    output_filename="bvpt",
    output_title=output_title))


@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    Three square emissive panels directly expose characteristically different noise textures:
    smooth single-layer 2D noise, detailed multi-layer 3D fBM, and warped 4D fBM.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
