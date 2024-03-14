import infra
from infra import image
from infra import renderer
from infra import paths

import pytest


res_dir = paths.test_resources() / "white_500W_rect_area_light_side"

bvpt_diffuse_sphere_case = infra.TestCase(__name__, "BVPT Diffuse Sphere", res_dir / "scene_bvpt_diffuse_sphere.p2")
bvpt_diffuse_sphere_case.output = "bvpt_diffuse_sphere"
bvpt_diffuse_sphere_case.debug_output = "bvpt_diffuse_sphere_error"
bvpt_diffuse_sphere_case.ref = "ref_diffuse_sphere"

bneept_diffuse_sphere_case = infra.TestCase(__name__, "BNEEPT Diffuse Sphere", res_dir / "scene_bneept_diffuse_sphere.p2")
bneept_diffuse_sphere_case.output = "bneept_diffuse_sphere"
bneept_diffuse_sphere_case.debug_output = "bneept_diffuse_sphere_error"
bneept_diffuse_sphere_case.ref = "ref_diffuse_sphere"

bvpt_glass_sphere_case = infra.TestCase(__name__, "BVPT Glass Sphere", res_dir / "scene_bvpt_glass_sphere.p2")
bvpt_glass_sphere_case.output = "bvpt_glass_sphere"
bvpt_glass_sphere_case.debug_output = "bvpt_glass_sphere_error"
bvpt_glass_sphere_case.ref = "ref_glass_sphere"

bneept_glass_sphere_case = infra.TestCase(__name__, "BNEEPT Glass Sphere", res_dir / "scene_bneept_glass_sphere.p2")
bneept_glass_sphere_case.output = "bneept_glass_sphere"
bneept_glass_sphere_case.debug_output = "bneept_glass_sphere_error"
bneept_glass_sphere_case.ref = "ref_glass_sphere"

@pytest.fixture(scope='module')
def ref_diffuse_sphere_img():
    img = image.read_pfm(res_dir / "ref_bvpt_diffuse_sphere_131072spp")
    img.save_plot(bvpt_diffuse_sphere_case.get_ref_path(), "Reference: BVPT 131072 spp", create_dirs=True)
    return img

@pytest.fixture(scope='module')
def ref_glass_sphere_img():
    img = image.read_pfm(res_dir / "ref_bvpt_glass_sphere_131072spp")
    img.save_plot(bvpt_glass_sphere_case.get_ref_path(), "Reference: BVPT 131072 spp", create_dirs=True)
    return img

@pytest.mark.parametrize("case, max_mse, max_re_avg", [
    pytest.param(bvpt_diffuse_sphere_case, 0.00048, 0.000176, id=bvpt_diffuse_sphere_case.get_name()), 
    pytest.param(bneept_diffuse_sphere_case, 0.00122, 0.000028, id=bneept_diffuse_sphere_case.get_name()),
    pytest.param(bvpt_glass_sphere_case, 0.00054, 0.0003, id=bvpt_glass_sphere_case.get_name()),
    pytest.param(bneept_glass_sphere_case, 0.00058, 0.00003, id=bneept_glass_sphere_case.get_name()),
])
def test_render(ref_diffuse_sphere_img, ref_glass_sphere_img, case, max_mse, max_re_avg):
    """
    This test places a bigger rectangular area light on the left, and objects of different materials are being
    illuminated. Since the light is quite large, all rendering methods should be able to perform this test.
    """
    if case is bvpt_diffuse_sphere_case or case is bneept_diffuse_sphere_case:
        ref_img = ref_diffuse_sphere_img
    else:
        ref_img = ref_glass_sphere_img

    process = renderer.open_default_render_process(case.get_scene_path(), case.get_output_path(), num_threads=10)
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
    