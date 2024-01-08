import infra
from infra import image
from infra import renderer
from infra import paths

import pytest


res_dir = paths.test_resources() / "white_100W_rect_area_light"

bvpt_case = infra.TestCase(__name__, "BVPT", res_dir / "scene_bvpt.p2")
bvpt_case.output = "bvpt"
bvpt_case.debug_output = "bvpt_error"
bvpt_case.ref = "ref"

bneept_case = infra.TestCase(__name__, "BNEEPT", res_dir /"scene_bneept.p2")
bneept_case.output = "bneept"
bneept_case.debug_output = "bneept_error"
bneept_case.ref = "ref"

sppm_case = infra.TestCase(__name__, "SPPM", res_dir /"scene_sppm.p2")
sppm_case.output = "sppm"
sppm_case.debug_output = "sppm_error"
sppm_case.ref = "ref"

@pytest.fixture(scope='module')
def ref_img():
    img = image.read_pfm(res_dir / "ref_bneept_8192spp")
    img.save_plot(bneept_case.get_ref_path(), "Reference: BNEEPT 8192 spp", create_dirs=True)
    return img

@pytest.mark.parametrize("case, max_mse, max_re_avg", [
    pytest.param(bvpt_case, 0.0036, 0.0008, id=bvpt_case.get_name()), 
    pytest.param(bneept_case, 0.0016, 0.0004, id=bneept_case.get_name()),
    pytest.param(sppm_case, 0.0144, 0.002, id=sppm_case.get_name()),
])
def test_render(ref_img, case, max_mse, max_re_avg):
    """
    A rectangular area light is shining the ground below it. The ground is diffusive (albedo = 50%).
    """
    process = renderer.open_default_render_process(case.get_scene_path(), case.get_output_path(), num_threads=4)
    process.run_and_wait()

    output_img = image.read_pfm(case.get_output_path())
    mse = image.mse_of(output_img, ref_img)
    re_avg = image.re_avg_of(output_img, ref_img)
    output_img.save_plot(case.get_output_path(), case.get_name() + " Output (MSE: %f, Δ: %f%%)" % (mse, re_avg * 100))

    output_img.values -= ref_img.values
    output_img.values *= 100
    output_img = output_img.to_summed_absolute()
    output_img.save_pseudocolor_plot(case.get_debug_output_path(), case.get_name() + " 100X Absolute Error")

    assert mse < max_mse
    assert abs(re_avg) < max_re_avg
    