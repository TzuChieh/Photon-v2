import infra
from infra import image
from infra import renderer
from infra import paths

import pytest


res_dir = paths.test_resources() / "white_100W_point_light"

bneept_case = infra.TestCase(__name__, "BNEEPT", res_dir / "scene_bneept.p2")
bneept_case.output = "bneept"
bneept_case.debug_output = "bneept_error"
bneept_case.ref = "ref"

sppm_case = infra.TestCase(__name__, "SPPM", res_dir / "scene_sppm.p2")
sppm_case.output = "sppm"
sppm_case.debug_output = "sppm_error"
sppm_case.ref = "ref"

@pytest.fixture(scope='module')
def ref_img():
    img = image.read_pfm(res_dir / "ref_bneept_65536spp")
    img.save_plot(bneept_case.get_ref_path(), "Reference: BNEEPT 65536 spp", create_dirs=True)
    return img

@pytest.mark.parametrize("case, max_mse, max_re_avg", [
    pytest.param(bneept_case, 0.000036, 0.001, id=bneept_case.get_name()),
    pytest.param(sppm_case, 0.000169, 0.005, id=sppm_case.get_name()),
])
def test_render(ref_img, case, max_mse, max_re_avg):
    """
    This test is similar to the "white_100W_small_rect_area_light" test, except that we are using a
    point light here. In Photon, we do not have true point light, they are just tiny spherical light.
    This test is not suitable to run using BVPT, as it may require way more than 10M samples to have
    proper convergence.
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
    output_img.save_pseudocolor_plot(case.get_debug_output_path(), case.get_name() + " 100X Absolute Error")

    assert mse < max_mse
    assert abs(re_avg) < max_re_avg
    