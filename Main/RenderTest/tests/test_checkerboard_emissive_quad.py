import infra
from infra import image
from infra import renderer
from infra import paths

import numpy as np
import pytest


res_dir = paths.test_resources() / "checkerboard_emissive_quad"

bvpt_case = infra.TestCase(__name__, "BVPT", res_dir / "scene_bvpt.p2")
bvpt_case.output = "bvpt"
bvpt_case.debug_output = "bvpt_error"
bvpt_case.ref = "ref"

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
    img = image.read_pfm(res_dir / "ref_bvpt_8192spp")
    img.save_plot(bvpt_case.get_ref_path(), "Reference: BVPT 8192 spp", create_dirs=True)
    return img

@pytest.mark.parametrize("case", [
    pytest.param(bvpt_case, id=bvpt_case.get_name()),
    pytest.param(bneept_case, id=bneept_case.get_name()),
    pytest.param(sppm_case, id=sppm_case.get_name()),
])
def test_render(ref_img, case):
    """
    A emissive quad is placed in front of the camera. The emission is textured with a
    checkerboard image. The quad will perfectly fit the rendered image (i.e., the rendered output
    should be identical to the checkboard image, with a different resolution).
    """
    process = renderer.open_default_render_process(case.get_scene_path(), case.get_output_path(), num_threads=4)
    process.run_and_wait()

    output_img = image.read_pfm(case.get_output_path())
    mse = image.mse_of(output_img, ref_img)
    output_img.save_plot(case.get_output_path(), case.get_name() + " Output (MSE: %f)" % mse)

    output_img.values -= ref_img.values
    output_img.values *= 100
    output_img = output_img.to_summed_absolute_components()
    output_img.save_pseudocolor_plot(case.get_debug_output_path(), case.get_name() + " 100X Absolute Error")

    assert mse < 0.0001
    