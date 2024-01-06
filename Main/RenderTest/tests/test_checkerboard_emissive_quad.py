import infra
from infra import image
from infra import renderer
from infra import paths

import numpy as np
import pytest


case = infra.TestCase()
case.name = "checkerboard_emissive_quad"
case.desc = ("A emissive quad is placed in front of the camera. The emission is textured with a "
    "checkerboard image. The quad will perfectly fit the rendered image (i.e., the rendered output "
    "should be identical to the checkboard image, with a different resolution).")
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
    img_path = paths.test_resources() / case.name / "ref_bvpt_8192spp"
    img = image.read_pfm(img_path)
    img.save_plot(case.refs[0], "Reference: BVPT 8192 spp", create_dirs=True)
    return img

@pytest.mark.parametrize("scene, output, debug_output", [
    (case.scenes[0], case.outputs[0], case.debug_outputs[0]), 
    (case.scenes[1], case.outputs[1], case.debug_outputs[1]),
    (case.scenes[2], case.outputs[2], case.debug_outputs[2]),
])
def test_render(ref_img, scene, output, debug_output):
    process = renderer.open_default_render_process(scene, output, num_threads=6)
    process.run_and_wait()

    output_img = image.read_pfm(output)
    rmse = image.rmse_of(output_img, ref_img)
    output_img.save_plot(output, output.name.upper() + " Output (RMSE: %f)" % rmse)

    output_img.values -= ref_img.values
    output_img.values *= 100
    output_img = output_img.to_summed_absolute()
    output_img.save_pseudocolor_plot(debug_output, output.name.upper() + " 100X Absolute Error")

    assert rmse < 0.01
    