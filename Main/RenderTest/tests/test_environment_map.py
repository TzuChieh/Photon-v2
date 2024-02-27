import infra
from infra import image
from infra import renderer
from infra import paths

import pytest


res_dir = paths.test_resources() / "environment_map"

debug_bvpt_sphere_case = infra.TestCase(__name__, "BVPT (debug map + sphere)", res_dir / "scene_debug_bvpt_sphere.p2")
debug_bvpt_sphere_case.output = "debug_bvpt_sphere"
debug_bvpt_sphere_case.debug_output = "debug_bvpt_sphere_error"
debug_bvpt_sphere_case.ref = "ref_debug_sphere"

debug_bvpt_sphere_shifted_case = infra.TestCase(__name__, "BVPT (debug map + sphere + shifted)", res_dir / "scene_debug_bvpt_sphere_shifted.p2")
debug_bvpt_sphere_shifted_case.output = "debug_bvpt_sphere_shifted"
debug_bvpt_sphere_shifted_case.debug_output = "debug_bvpt_sphere_shifted_error"
debug_bvpt_sphere_shifted_case.ref = "ref_debug_sphere"
debug_bvpt_sphere_shifted_case.case_msg = (
    "All settings are the same with \"BVPT (debug map + sphere)\" case, except the sphere and camera are shifted "
    "2000 units in +x. The result should be the same as our environment map depends on view direction only.")

debug_bneept_sphere_case = infra.TestCase(__name__, "BNEEPT (debug map + sphere)", res_dir / "scene_debug_bneept_sphere.p2")
debug_bneept_sphere_case.output = "debug_bneept_sphere"
debug_bneept_sphere_case.debug_output = "debug_bneept_sphere_error"
debug_bneept_sphere_case.ref = "ref_debug_sphere"

debug_bneept_sphere_shifted_case = infra.TestCase(__name__, "BNEEPT (debug map + sphere + shifted)", res_dir / "scene_debug_bneept_sphere_shifted.p2")
debug_bneept_sphere_shifted_case.output = "debug_bneept_sphere_shifted"
debug_bneept_sphere_shifted_case.debug_output = "debug_bneept_sphere_shifted_error"
debug_bneept_sphere_shifted_case.ref = "ref_debug_sphere"
debug_bneept_sphere_shifted_case.case_msg = (
    "All settings are the same with \"BNEEPT (debug map + sphere)\" case, except the sphere and camera are shifted "
    "2000 units in +x. The result should be the same as our environment map depends on view direction only.")

white_bvpt_sphere_case = infra.TestCase(__name__, "BVPT (white map + sphere)", res_dir / "scene_white_bvpt_sphere.p2")
white_bvpt_sphere_case.output = "white_bvpt_sphere"
white_bvpt_sphere_case.debug_output = "white_bvpt_sphere_error"
white_bvpt_sphere_case.ref = "ref_white"
white_bvpt_sphere_case.case_msg = (
    "Effectively a white furnace test.")

white_bneept_sphere_case = infra.TestCase(__name__, "BNEEPT (white map + sphere)", res_dir / "scene_white_bneept_sphere.p2")
white_bneept_sphere_case.output = "white_bneept_sphere"
white_bneept_sphere_case.debug_output = "white_bneept_sphere_error"
white_bneept_sphere_case.ref = "ref_white"
white_bneept_sphere_case.case_msg = (
    "Effectively a white furnace test.")

@pytest.fixture(scope='module')
def ref_white_img():
    img = image.Image(128, 64, 3)
    img.fill(1.0)
    img.save_plot(white_bvpt_sphere_case.get_ref_path(), "Reference: Fully white (1.0)", create_dirs=True)
    return img

@pytest.fixture(scope='module')
def ref_debug_sphere_img():
    img = image.read_pfm(res_dir / "ref_debug_bvpt_sphere_16384spp")
    img.save_plot(debug_bvpt_sphere_case.get_ref_path(), "Reference: Debug map + sphere, 16384 spp", create_dirs=True)
    return img

@pytest.mark.parametrize("case, max_mse, max_re_avg", [
    pytest.param(debug_bvpt_sphere_case, 0.0001, 0.002, id=debug_bvpt_sphere_case.get_name()),
    pytest.param(debug_bvpt_sphere_shifted_case, 0.0001, 0.002, id=debug_bvpt_sphere_shifted_case.get_name()),
    pytest.param(debug_bneept_sphere_case, 0.0001, 0.0003, id=debug_bneept_sphere_case.get_name()),
    pytest.param(debug_bneept_sphere_shifted_case, 0.0001, 0.0003, id=debug_bneept_sphere_shifted_case.get_name()),
    pytest.param(white_bvpt_sphere_case, 1e-10, 1e-10, id=white_bvpt_sphere_case.get_name()),
    pytest.param(white_bneept_sphere_case, 1e-10, 1e-10, id=white_bneept_sphere_case.get_name()),
])
def test_render(ref_white_img, ref_debug_sphere_img, case, max_mse, max_re_avg):
    """
    An object is being illuminated by an environment map.
    """
    is_white_case = case is white_bvpt_sphere_case or case is white_bneept_sphere_case

    process = renderer.open_default_render_process(case.get_scene_path(), case.get_output_path(), num_threads=4)
    process.run_and_wait()

    ref_img = ref_white_img if is_white_case else ref_debug_sphere_img

    output_img = image.read_pfm(case.get_output_path())
    mse = image.mse_of(output_img, ref_img)
    re_avg = image.re_avg_of(output_img, ref_img)
    output_img.save_plot(case.get_output_path(), case.get_name() + " Output (MSE: %f, Δ: %f%%)" % (mse, re_avg * 100))

    output_img.values -= ref_img.values
    output_img.values *= 100
    output_img = output_img.to_summed_absolute_components()
    output_img.save_pseudocolor_plot(case.get_debug_output_path(), case.get_name() + " 100X Absolute Error")

    assert mse < max_mse
    assert abs(re_avg) < max_re_avg
    