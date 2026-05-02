import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "gray_furnace_box"
suite = infra.RenderTestSuite(__name__, res_dir)

def energy_loss_image(img):
    debug_img = img.to_averaged_component()
    debug_img.values = (debug_img.values - 0.5) / 0.5
    debug_img.values *= 100
    return debug_img

def energy_loss_title(case):
    return case.name + " Energy Loss (%)"

energy_loss_verifier = infra.PseudocolorPlotVerifier(
    transform=energy_loss_image,
    title=energy_loss_title,
    color_min=-1.2,
    color_max=1.2,
    color_map='bwr')

small_box_case_msg = (
    "All settings are the same with \"BNEEPT Diffuse Sphere\" case, except the furnace is a smaller box "
    "(2000 -> 40, 50X reduction).")

for case_name, output_name, scene_name, num_threads, case_msg in [
    ("BVPT Diffuse Sphere", "bvpt_diffuse_sphere", "scene_bvpt_diffuse_sphere.p2", 6, ""),
    ("BNEEPT Diffuse Sphere", "bneept_diffuse_sphere", "scene_bneept_diffuse_sphere.p2", 6, ""),
    ("BNEEPT Diffuse Sphere (Small Box)", "bneept_diffuse_sphere_small_box", "scene_bneept_diffuse_sphere_small_box.p2", 6, small_box_case_msg),
    ("SPPM Diffuse Sphere", "sppm_diffuse_sphere", "scene_sppm_diffuse_sphere.p2", 12, ""),
    ("BNEEPT Glass Sphere", "bneept_glass_sphere", "scene_bneept_glass_sphere.p2", 6, ""),
    ("BNEEPT Glass Cube", "bneept_glass_cube", "scene_bneept_glass_cube.p2", 6, ""),
    ("PPPM Glass Cube", "pppm_glass_cube", "scene_pppm_glass_cube.p2", 12, "")
    ]:
    suite.add_case(infra.RenderCase(
        case_name,
        res_dir / scene_name,
        infra.RendererConfig(num_threads=num_threads),
        [
            infra.MeanDiffVerifier(expected_value=0.5, threshold=1e-3),
            energy_loss_verifier
        ],
        output_filename=output_name,
        case_msg=case_msg))

@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    The classical white furnace test, with medium gray background to better judge energy loss/gain. There is
    an object placed in the middle. If there is no energy loss in the target model, then the output image
    should be 0.5 for all pixels.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
