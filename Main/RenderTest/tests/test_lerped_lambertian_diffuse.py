import infra
from infra import image
from infra import renderer
from infra import paths

import pytest


res_dir = paths.test_resources() / "lerped_lambertian_diffuse"

bvpt_case1 = infra.TestCase(__name__, "BVPT (factor = 50 percent)", res_dir / "scene_factor0p5_bvpt.p2")
bvpt_case1.output = "bvpt_factor0p5"
bvpt_case1.debug_output = "bvpt_factor0p5_error"
bvpt_case1.ref = "ref"

bvpt_case2 = infra.TestCase(__name__, "BVPT (factor = 80 percent)", res_dir / "scene_factor0p8_bvpt.p2")
bvpt_case2.output = "bvpt_factor0p8"
bvpt_case2.debug_output = "bvpt_factor0p8_error"
bvpt_case2.ref = "ref"

bvpt_case3 = infra.TestCase(__name__, "BVPT (factor = 0 percent)", res_dir / "scene_factor0p0_bvpt.p2")
bvpt_case3.output = "bvpt_factor0p0"
bvpt_case3.debug_output = "bvpt_factor0p0_error"
bvpt_case3.ref = "ref"
bvpt_case3.case_msg = "This is a corner case where the lerping factor is 0 (0 * material_0 + 1 * material_0)."

bneept_case1 = infra.TestCase(__name__, "BNEEPT (factor = 50 percent)", res_dir / "scene_factor0p5_bneept.p2")
bneept_case1.output = "bneept_factor0p5"
bneept_case1.debug_output = "bneept_factor0p5_error"
bneept_case1.ref = "ref"

bneept_case2 = infra.TestCase(__name__, "BNEEPT (factor = 0 percent)", res_dir / "scene_factor0p0_bneept.p2")
bneept_case2.output = "bneept_factor0p0"
bneept_case2.debug_output = "bneept_factor0p0_error"
bneept_case2.ref = "ref"
bneept_case2.case_msg = "This is a corner case where the lerping factor is 0 (0 * material_0 + 1 * material_0)."

@pytest.fixture(scope='module')
def ref_img():
    img = image.read_pfm(res_dir / "ref_no_lerp_bvpt_65536spp")
    img.save_plot(bvpt_case1.get_ref_path(), "Reference: BVPT (no lerp) 65536 spp", create_dirs=True)
    return img

@pytest.mark.parametrize("case, max_mse, max_re_avg", [
    pytest.param(bvpt_case1, 0.00012, 0.00082, id=bvpt_case1.get_name()),
    pytest.param(bvpt_case2, 0.00012, 0.00036, id=bvpt_case2.get_name()),
    pytest.param(bvpt_case3, 0.00012, 0.00036, id=bvpt_case3.get_name()),
    pytest.param(bneept_case1, 0.00012, 0.00080, id=bneept_case1.get_name()),
    pytest.param(bneept_case2, 0.00012, 0.00052, id=bneept_case2.get_name()),
])
def test_render(ref_img, case, max_mse, max_re_avg):
    """
    A sphere with lerped Lambertian diffuse of different factors, comparing against a non-lerped (albedo = 
    100%) reference. The ground is non-lerped diffusive (albedo = 50%).
    """
    process = renderer.open_default_render_process(case.get_scene_path(), case.get_output_path(), num_threads=4)
    process.run_and_wait()

    output_img = image.read_pfm(case.get_output_path())
    mse = image.mse_of(output_img, ref_img)
    re_avg = image.re_avg_of(output_img, ref_img)
    output_img.save_plot(case.get_output_path(), case.get_name() + " Output (MSE: %f, Δ: %f%%)" % (mse, re_avg * 100))

    output_img.values -= ref_img.values
    output_img.values *= 100
    output_img = output_img.to_summed_absolute_components()
    output_img.save_pseudocolor_plot(case.get_debug_output_path(), case.get_name() + " 100X Absolute Error", color_max=70)

    assert mse < max_mse
    assert abs(re_avg) < max_re_avg
    