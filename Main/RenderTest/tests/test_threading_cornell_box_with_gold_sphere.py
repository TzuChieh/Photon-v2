import infra
from infra import image
from infra import renderer
from infra import paths

import pytest


res_dir = paths.test_resources() / "cornell_box_with_gold_sphere"
num_threads_to_test = [1, 2, 5, 8, 17, 64, 100]

cases = []
for i, t in enumerate(num_threads_to_test):
    _case = infra.TestCase(__name__, "BNEEPT (%d) %dT" % (i + 1, t), res_dir / "scene_bneept.p2")
    _case.output = "bneept_%dt" % t
    _case.debug_output = "bneept_error_%dt" % t
    _case.ref = "ref"
    cases.append(_case)

@pytest.fixture(scope='module')
def ref_img():
    img = image.read_pfm(res_dir / "ref_bneept_32768spp")
    img.save_plot(cases[0].get_ref_path(), "Reference: BNEEPT 32768 spp", create_dirs=True)
    return img

@pytest.mark.parametrize("case, t",
    [p for p in zip(cases, num_threads_to_test)],
    ids=[case.get_name() for case in cases]
)
def test_render(ref_img, case, t):
    """
    This test is for testing threaded rendering on a simple scene.
    """
    process = renderer.open_default_render_process(case.get_scene_path(), case.get_output_path(), num_threads=t)
    process.run_and_wait()

    output_img = image.read_pfm(case.get_output_path())
    mse = image.mse_of(output_img, ref_img)
    re_avg = image.re_avg_of(output_img, ref_img)
    output_img.save_plot(case.get_output_path(), case.get_name() + " Output (MSE: %f, Δ: %f%%)" % (mse, re_avg * 100))

    output_img.values -= ref_img.values
    output_img.values *= 100
    output_img = output_img.to_summed_absolute_components()
    output_img.save_pseudocolor_plot(case.get_debug_output_path(), case.get_name() + " 100X Absolute Error")

    assert mse < 0.0016
    assert abs(re_avg) < 0.0025
    