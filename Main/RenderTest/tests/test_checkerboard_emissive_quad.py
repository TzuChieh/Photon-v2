import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "checkerboard_emissive_quad"
suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=4)
ref_path = res_dir / "ref_bvpt_8192spp_beauty"
bvpt_ref_var_path = res_dir / "ref_bvpt_8192spp_var"
bneept_ref_var_path = res_dir / "ref_bneept_8192spp_var"

def output_title(case, metrics):
    return "%s Output (MSE: %f)" % (case.name, metrics["mse"])

visual_error_verifier = infra.VisualErrorVerifier(
    ref=ref_path,
    error_scale=100.0,
    ref_title="Reference: BVPT 8192 spp")

for case_name, output_name, scene_name, ref_var_path, z_sample_count in [
    ("BVPT", "bvpt", "scene_bvpt.p2", bvpt_ref_var_path, 512),
    ("BNEEPT", "bneept", "scene_bneept.p2", bneept_ref_var_path, 512),
    ("SPPM", "sppm", "scene_sppm.p2", None, None)
    ]:
    verifiers = [
        infra.MSEVerifier(ref=ref_path, threshold=0.0001),
        visual_error_verifier]
    if z_sample_count is not None:
        verifiers.append(infra.ZTestVerifier(
            ref=ref_path, ref_variance=ref_var_path, sample_count=z_sample_count))

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
    A emissive quad is placed in front of the camera. The emission is textured with a
    checkerboard image. The quad will perfectly fit the rendered image (i.e., the rendered output
    should be identical to the checkboard image, with a different resolution).
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
