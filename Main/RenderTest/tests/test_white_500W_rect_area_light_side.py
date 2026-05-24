import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "white_500W_rect_area_light_side"
suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=10)
diffuse_ref_path = res_dir / "ref_bvpt_diffuse_sphere_131072spp_0"
glass_ref_path = res_dir / "ref_bvpt_glass_sphere_131072spp_0"
diffuse_ref_var_path = res_dir / "ref_bvpt_diffuse_sphere_131072spp_1"
glass_ref_var_path = res_dir / "ref_bvpt_glass_sphere_131072spp_1"

def output_title(case, metrics):
    return "%s Output (MSE: %f, Δ: %f%%)" % (case.name, metrics["mse"], metrics["rel_mean"] * 100)

diffuse_visual_error_verifier = infra.VisualErrorVerifier(
    ref=diffuse_ref_path,
    error_scale=100.0,
    ref_output_filename="ref_diffuse_sphere",
    ref_title="Reference: BVPT 131072 spp")
glass_visual_error_verifier = infra.VisualErrorVerifier(
    ref=glass_ref_path,
    error_scale=100.0,
    ref_output_filename="ref_glass_sphere",
    ref_title="Reference: BVPT 131072 spp")

for case_name, output_name, scene_name, ref_path, ref_var_path, visual_error_verifier, sample_count, max_mse, max_rel_mean in [
    ("BVPT Diffuse Sphere", "bvpt_diffuse_sphere", "scene_bvpt_diffuse_sphere.p2", diffuse_ref_path, diffuse_ref_var_path, diffuse_visual_error_verifier, 2500, 0.00048, 0.0004),
    ("BNEEPT Diffuse Sphere", "bneept_diffuse_sphere", "scene_bneept_diffuse_sphere.p2", diffuse_ref_path, diffuse_ref_var_path, diffuse_visual_error_verifier, 500, 0.00122, 0.00068),
    ("BVPT Glass Sphere", "bvpt_glass_sphere", "scene_bvpt_glass_sphere.p2", glass_ref_path, glass_ref_var_path, glass_visual_error_verifier, 2500, 0.00054, 0.00064),
    ("BNEEPT Glass Sphere", "bneept_glass_sphere", "scene_bneept_glass_sphere.p2", glass_ref_path, glass_ref_var_path, glass_visual_error_verifier, 1500, 0.00058, 0.00064)
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
    This test places a bigger rectangular area light on the left, and objects of different materials are being
    illuminated. Since the light is quite large, all rendering methods should be able to perform this test.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
