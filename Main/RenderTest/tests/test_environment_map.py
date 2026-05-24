import infra
from infra import image
from infra import paths
import pytest


res_dir = paths.test_resources() / "environment_map"
suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=4)
debug_ref_path = res_dir / "ref_debug_bvpt_sphere_16384spp_0"
white_ref_img = image.Image(128, 64, 3)
white_ref_img.fill(1.0)

def output_title(case, metrics):
    return "%s Output (MSE: %f, Δ: %f%%)" % (case.name, metrics["mse"], metrics["rel_mean"] * 100)

debug_visual_error_verifier = infra.VisualErrorVerifier(
    ref=debug_ref_path,
    error_scale=100.0,
    ref_output_filename="ref_debug_sphere",
    ref_title="Reference: Debug map + sphere, 16384 spp")
white_visual_error_verifier = infra.VisualErrorVerifier(
    ref=white_ref_img,
    error_scale=100.0,
    ref_output_filename="ref_white",
    ref_title="Reference: Fully white (1.0)")

for case_name, output_name, scene_name, ref, visual_error_verifier, max_mse, max_rel_mean, case_msg in [
    ("BVPT (debug map + sphere)", "debug_bvpt_sphere", "scene_debug_bvpt_sphere.p2",
     debug_ref_path, debug_visual_error_verifier, 0.0001, 0.002,
     ""),
    ("BVPT (debug map + sphere + shifted)", "debug_bvpt_sphere_shifted", "scene_debug_bvpt_sphere_shifted.p2",
     debug_ref_path, debug_visual_error_verifier, 0.0001, 0.002,
     "All settings are the same with \"BVPT (debug map + sphere)\" case, except the sphere and camera are shifted "
     "2000 units in +x. The result should be the same as our environment map depends on view direction only."),
    ("BNEEPT (debug map + sphere)", "debug_bneept_sphere", "scene_debug_bneept_sphere.p2",
     debug_ref_path, debug_visual_error_verifier, 0.0001, 0.0003,
     ""),
    ("BNEEPT (debug map + sphere + shifted)", "debug_bneept_sphere_shifted", "scene_debug_bneept_sphere_shifted.p2",
     debug_ref_path, debug_visual_error_verifier, 0.0001, 0.0006,
     "All settings are the same with \"BNEEPT (debug map + sphere)\" case, except the sphere and camera are shifted "
     "2000 units in +x. The result should be the same as our environment map depends on view direction only."),
    ("BVPT (white map + sphere)", "white_bvpt_sphere", "scene_white_bvpt_sphere.p2",
     white_ref_img, white_visual_error_verifier, 1e-10, 1e-10,
     "Effectively a white furnace test."),
    ("BNEEPT (white map + sphere)", "white_bneept_sphere", "scene_white_bneept_sphere.p2",
     white_ref_img, white_visual_error_verifier, 1e-10, 1e-10,
     "Effectively a white furnace test."),
    ("PPPM (white map + plane)", "white_pppm_plane", "scene_white_pppm_plane.p2",
     white_ref_img, white_visual_error_verifier, 0.002, 0.0106,
     "Effectively a white furnace test. The receiver is placed fairly close to the plane, looking at the horizon "
     "(forms grazing angles). The environment sphere is also shifted and rotated, which should not affect the result.")
    ]:
    suite.add_case(infra.RenderCase(
        case_name,
        res_dir / scene_name,
        renderer_config,
        [
            infra.MSEVerifier(ref=ref, threshold=max_mse),
            infra.RelMeanVerifier(ref=ref, threshold=max_rel_mean),
            visual_error_verifier
        ],
        output_filename=output_name,
        output_title=output_title,
        case_msg=case_msg))

@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    An object is being illuminated by an environment map.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
