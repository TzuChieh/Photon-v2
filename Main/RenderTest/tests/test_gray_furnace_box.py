import infra
from infra import image
from infra import renderer
from infra import paths

import numpy as np
import pytest


res_dir = paths.test_resources() / "gray_furnace_box"

bvpt_diffuse_sphere_case = infra.TestCase(__name__, "BVPT Diffuse Sphere", res_dir / "scene_bvpt_diffuse_sphere.p2")
bvpt_diffuse_sphere_case.output = "bvpt_diffuse_sphere"
bvpt_diffuse_sphere_case.debug_output = "bvpt_diffuse_sphere_error"

bneept_diffuse_sphere_case = infra.TestCase(__name__, "BNEEPT Diffuse Sphere", res_dir / "scene_bneept_diffuse_sphere.p2")
bneept_diffuse_sphere_case.output = "bneept_diffuse_sphere"
bneept_diffuse_sphere_case.debug_output = "bneept_diffuse_sphere_error"

bneept_diffuse_sphere_small_box_case = infra.TestCase(__name__, "BNEEPT Diffuse Sphere (Small Box)", res_dir / "scene_bneept_diffuse_sphere_small_box.p2")
bneept_diffuse_sphere_small_box_case.output = "bneept_diffuse_sphere_small_box"
bneept_diffuse_sphere_small_box_case.debug_output = "bneept_diffuse_sphere_small_box_error"
bneept_diffuse_sphere_small_box_case.case_msg = (
    "All settings are the same with \"BNEEPT Diffuse Sphere\" case, except the furnace is a smaller box "
    "(2000 -> 40, 50X reduction).")

sppm_diffuse_sphere_case = infra.TestCase(__name__, "SPPM Diffuse Sphere", res_dir / "scene_sppm_diffuse_sphere.p2")
sppm_diffuse_sphere_case.output = "sppm_diffuse_sphere"
sppm_diffuse_sphere_case.debug_output = "sppm_diffuse_sphere_error"

bneept_glass_sphere_case = infra.TestCase(__name__, "BNEEPT Glass Sphere", res_dir / "scene_bneept_glass_sphere.p2")
bneept_glass_sphere_case.output = "bneept_glass_sphere"
bneept_glass_sphere_case.debug_output = "bneept_glass_sphere_error"

bneept_glass_cube_case = infra.TestCase(__name__, "BNEEPT Glass Cube", res_dir / "scene_bneept_glass_cube.p2")
bneept_glass_cube_case.output = "bneept_glass_cube"
bneept_glass_cube_case.debug_output = "bneept_glass_cube_error"

pppm_glass_cube_case = infra.TestCase(__name__, "PPPM Glass Cube", res_dir / "scene_pppm_glass_cube.p2")
pppm_glass_cube_case.output = "pppm_glass_cube"
pppm_glass_cube_case.debug_output = "pppm_glass_cube_error"

@pytest.mark.parametrize("case", [
    pytest.param(bvpt_diffuse_sphere_case, id=bvpt_diffuse_sphere_case.get_name()),
    pytest.param(bneept_diffuse_sphere_case, id=bneept_diffuse_sphere_case.get_name()),
    pytest.param(bneept_diffuse_sphere_small_box_case, id=bneept_diffuse_sphere_small_box_case.get_name()),
    pytest.param(sppm_diffuse_sphere_case, id=sppm_diffuse_sphere_case.get_name()),
    pytest.param(bneept_glass_sphere_case, id=bneept_glass_sphere_case.get_name()),
    pytest.param(bneept_glass_cube_case, id=bneept_glass_cube_case.get_name()),
    pytest.param(pppm_glass_cube_case, id=pppm_glass_cube_case.get_name()),
])
def test_render(case):
    """
    The classical white furnace test, with medium gray background to better judge energy loss/gain. There is
    an object placed in the middle. If there is no energy loss in the target model, then the output image
    should be 0.5 for all pixels.
    """
    t = 12 if case is sppm_diffuse_sphere_case or case is pppm_glass_cube_case else 6
    process = renderer.open_default_render_process(case.get_scene_path(), case.get_output_path(), num_threads=t)
    process.run_and_wait()

    img = image.read_pfm(case.get_output_path())
    img.save_plot(case.get_output_path(), case.get_name() + " Output")

    mean_diff = np.mean(img.values) - 0.5
    case.debug_msg = "mean diff = %.8f, max pixel = %.8f, min pixel = %.8f" % (
        mean_diff, np.max(img.values), np.min(img.values))

    img = img.to_averaged_component()
    img.values = (img.values - 0.5) / 0.5
    img.values *= 100
    img.save_pseudocolor_plot(
        case.get_debug_output_path(), 
        case.get_name() + " Energy Loss (%)",
        color_min=-1.2,
        color_max=1.2,
        color_map='bwr')

    max_mean_diff = 1e-3
    assert abs(mean_diff) < max_mean_diff
    