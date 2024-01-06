import infra
from infra import image
from infra import renderer
from infra import paths

import pytest


case = infra.TestCase()
case.name = "white_100W_rect_area_light"
case.desc = ("A rectangular area light is shining the ground below it. The ground is diffusive (albedo = 50%).")
case.scenes = [
    paths.test_resources() / case.name / "scene_bvpt.p2",
    paths.test_resources() / case.name / "scene_bneept.p2",
    paths.test_resources() / case.name / "scene_sppm.p2",
]
case.outputs = [
    paths.test_output() / case.name / "bvpt",
    paths.test_output() / case.name / "bneept",
    paths.test_output() / case.name / "sppm",
]
case.debug_outputs = [
    paths.test_output() / case.name / "bvpt_error",
    paths.test_output() / case.name / "bneept_error",
    paths.test_output() / case.name / "sppm_error",
]
case.refs = [
    paths.test_output() / case.name / "ref"
]

@pytest.fixture(scope='module')
def ref_img():
    img_path = paths.test_resources() / case.name / "ref_bneept_8192spp"
    img = image.read_pfm(img_path)
    img.save_plot(case.refs[0], "Reference: BNEEPT 8192 spp", create_dirs=True)
    return img

@pytest.mark.parametrize("scene, output, debug_output, max_rmse, max_re_avg", [
    (case.scenes[0], case.outputs[0], case.debug_outputs[0], 0.06, 0.0008), 
    (case.scenes[1], case.outputs[1], case.debug_outputs[1], 0.04, 0.0004),
    (case.scenes[2], case.outputs[2], case.debug_outputs[1], 0.12, 0.002),
])
def test_render(ref_img, scene, output, debug_output, max_rmse, max_re_avg):
    process = renderer.open_default_render_process(scene, output, num_threads=6)
    process.run_and_wait()

    output_img = image.read_pfm(output)
    rmse = image.rmse_of(output_img, ref_img)
    re_avg = image.re_avg_of(output_img, ref_img)
    output_img.save_plot(output, output.name.upper() + " Output (RMSE: %f, Δ: %f%%)" % (rmse, re_avg * 100))

    output_img.values -= ref_img.values
    output_img.values *= 100
    output_img = output_img.to_summed_absolute()
    output_img.save_pseudocolor_plot(debug_output, output.name.upper() + " 100X Absolute Error")

    assert rmse < max_rmse
    assert abs(re_avg) < max_re_avg
    