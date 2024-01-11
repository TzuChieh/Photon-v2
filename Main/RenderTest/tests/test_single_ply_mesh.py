import infra
from infra import image
from infra import renderer
from infra import paths

import pytest


res_dir = paths.test_resources() / "single_ply_mesh"

quad_bvpt_ascii_case = infra.TestCase(__name__, "Quad (BVPT, ASCII)", res_dir / "quad_bvpt_ascii.p2")
quad_bvpt_ascii_case.output = "quad_bvpt_ascii"
quad_bvpt_ascii_case.debug_output = "quad_bvpt_ascii_error"
quad_bvpt_ascii_case.ref = "ref_quad"

quad_bvpt_case = infra.TestCase(__name__, "Quad (BVPT)", res_dir / "quad_bvpt.p2")
quad_bvpt_case.output = "quad_bvpt"
quad_bvpt_case.debug_output = "quad_bvpt_error"
quad_bvpt_case.ref = "ref_quad"

quad_bneept_ascii_case = infra.TestCase(__name__, "Quad (BNEEPT, ASCII)", res_dir / "quad_bneept_ascii.p2")
quad_bneept_ascii_case.output = "quad_bneept_ascii"
quad_bneept_ascii_case.debug_output = "quad_bneept_ascii_error"
quad_bneept_ascii_case.ref = "ref_quad"

quad_bneept_case = infra.TestCase(__name__, "Quad (BNEEPT)", res_dir / "quad_bneept.p2")
quad_bneept_case.output = "quad_bneept"
quad_bneept_case.debug_output = "quad_bneept_error"
quad_bneept_case.ref = "ref_quad"

quad_sppm_case = infra.TestCase(__name__, "Quad (SPPM)", res_dir / "quad_sppm.p2")
quad_sppm_case.output = "quad_sppm"
quad_sppm_case.debug_output = "quad_sppm_error"
quad_sppm_case.ref = "ref_quad"

suzanne_bvpt_case = infra.TestCase(__name__, "Suzanne (BVPT)", res_dir / "suzanne_bvpt.p2")
suzanne_bvpt_case.output = "suzanne_bvpt"
suzanne_bvpt_case.debug_output = "suzanne_bvpt_error"
suzanne_bvpt_case.ref = "ref_suzanne"

suzanne_bneept_case = infra.TestCase(__name__, "Suzanne (BNEEPT)", res_dir / "suzanne_bneept.p2")
suzanne_bneept_case.output = "suzanne_bneept"
suzanne_bneept_case.debug_output = "suzanne_bneept_error"
suzanne_bneept_case.ref = "ref_suzanne"

@pytest.fixture(scope='module')
def ref_quad_img():
    img = image.read_pfm(res_dir / "ref_quad_bvpt_16384spp")
    img.save_plot(quad_bvpt_case.get_ref_path(), "Reference: BVPT 16384 spp", create_dirs=True)
    return img

@pytest.fixture(scope='module')
def ref_suzanne_img():
    img = image.read_pfm(res_dir / "ref_suzanne_bneept_16384spp")
    img.save_plot(suzanne_bvpt_case.get_ref_path(), "Reference: Suzanne 16384 spp", create_dirs=True)
    return img

@pytest.mark.parametrize("case, max_mse, max_re_avg", [
    pytest.param(quad_bvpt_ascii_case, 0.000074, 0.002, id=quad_bvpt_ascii_case.get_name()),
    pytest.param(quad_bvpt_case, 0.000074, 0.002, id=quad_bvpt_case.get_name()),
    pytest.param(quad_bneept_ascii_case, 0.000074, 0.002, id=quad_bneept_ascii_case.get_name()),
    pytest.param(quad_bneept_case, 0.000074, 0.002, id=quad_bneept_case.get_name()),
    pytest.param(quad_sppm_case, 0.0014, 0.003, id=quad_sppm_case.get_name()),
    pytest.param(suzanne_bvpt_case, 0.00007, 0.001, id=suzanne_bvpt_case.get_name()),
    pytest.param(suzanne_bneept_case, 0.00007, 0.001, id=suzanne_bneept_case.get_name()),
])
def test_render(ref_quad_img, ref_suzanne_img, case, max_mse, max_re_avg):
    """
    A single .ply mesh is placed between a large area light and diffusive ground. The mesh itself is diffusive.
    """
    is_suzzane_case = case is suzanne_bvpt_case or case is suzanne_bneept_case

    t = 12 if is_suzzane_case else 6 
    process = renderer.open_default_render_process(case.get_scene_path(), case.get_output_path(), num_threads=t)
    process.run_and_wait()

    ref_img = ref_suzanne_img if is_suzzane_case else ref_quad_img

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
    